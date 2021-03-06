
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      rainflux.c
**
**  FUNCTION:  void rainflux()
**
**  AUTHOR:    Melannie Hartman
**             Bill Parton
**
**  PURPOSE:   Add rain and snowmelt into the soil profile.
**
**  INPUTS:
**    impedance      - flag, number of lowest frozen or otherwise impermeable layer
**    infil_capacity - infiltration capacity (cm/sec)
**    infil_time     - length of time, in hours, over which the water is
**                     infiltrated into the soil
**    numlyrs        - total number of layers in the soil water model soil
**                     profile
**    satcond[]      - saturated hydraulic conductivity by layer (cm/sec)
**    soiltavg[]     - average soil temperature by layer (deg C)
**    swc[]          - soil water content by layer (cm H2O)
**    swcfc[]        - soil water content at field capacity  (cm H2O for layer)
**    thetas_bd[]    - saturation volumetric soil water content by layer (cm H2O/cm)
**    watrinput      - the amount of rain and snowmelt (cm)
**    width[]        - the thickness of soil water model layers (cm)
**
**  GLOBAL VARIABLES:
**    FRZSOIL        - temperature at which soil is considered frozen (deg C)
**                     (-1.0)
**    MXSWLYR        - maximum number of soil water model layers (21)
**    MIN_FRZN_COND  - minimum hydraulic conductivity when soil is frozen
**                     (cm/sec) (equivalent to about 1cm/day) (0.00001)
**    SEC_PER_HOUR   - number of seconds in an hour (3600)
**
**  LOCAL VARIABLES:
**    debug           - flag to set debugging mode, 0 = off, 1 = on
**    dt              - the amount of time elapsed to add water to a layer
**                      (sec)
**    ilyr            - current layer in the soil profile
**    infil_rate      - the rate at which water is being added to the profile
**                      (cm/sec) the infiltration rate to layer "i" is the
**                      minimum of the infiltration capacities of layers 0..i
**    input_intensity - the rate at which water is arriving (cm/sec)
**    tdiff           - difference between cumtime and
**                      infil_time*SEC_PER_HOUR,
**                      used to check for error condition
**    theta           - the volumetric swc of a layer (frac)
**    watrin          - the amount of water added to a layer (cm)
**    watrleft        - water input which has not infiltrated yet (cm)
**    wdiff           - amount of water infiltrated plus amount of water that
**                      did not infiltrate minus the amount of rain and
**                      snowmelt (cuminfl + runoff - watrinput),
**                      used to check for error condition
**    wtosat          - the amount of water which could be added to a layer to
**                      bring it to saturation (cm).
**
**  OUTPUTS:
**    cuminfl    - amount of water that infiltrated (cm)
**    cumtime    - time elapsed to infiltrate water (sec)
**    drain_out  - drainage out of the bottom of the soil profile (cm H2O)
**    runoff     - amount of water (rain or snowmelt) which did not infiltrate
**                 soil profile (cm)
**    swc[]      - soil water content by layer (cm H2O)
**    wfluxout[] - the amount of water moving through the bottom of a soil
**                 layer (cm H2O) (positive is downward, negative is upward)
**
**  CALLED BY:
**    h2oflux()
**
**  CALLS:
**    hwdrain()
**
**  NOTES:
**    thetas changed to thetas_bd. -cindyk 9/18/00
**
*****************************************************************************/

#include "mssg.h"
#include "soilwater.h"
#include <math.h>
#include <stdlib.h>

    void rainflux(float watrinput, int infil_time, double swc[MXSWLYR],
                  float thetas_bd[MXSWLYR], float width[MXSWLYR],
                  float satcond[MXSWLYR], float swcfc[MXSWLYR],
                  float infil_capacity, float soiltavg[MXSWLYR], int numlyrs,
                  double *runoff, double *cumtime, double *cuminfl,
                  double *drain_out, float wfluxout[MXSWLYR], int *impedance)
    {

      double watrleft;
      double wtosat;
      double watrin;
      double input_intensity;
      double infil_rate;
      double dt;
      double tdiff;
      double wdiff;
      int    ilyr;
      int    debug = 0;
/*      int    impedance; */
      float  theta;

      *impedance = 0;
      ilyr = 0;
      *cumtime = 0;
      *cuminfl = 0;
      *runoff = 0;

      watrleft = watrinput;
      input_intensity = watrinput/(infil_time*SEC_PER_HOUR);

      if (debug) {
        printf("Rainflux: infil_capacity = %12.10f\n", infil_capacity);
        printf("Rainflux: input_intensity = %12.10f\n", input_intensity);
        printf("Rainflux: infil_time = %1d\n", infil_time);
      }

      theta = (float)swc[0]/width[0];

      /* Compute the rate at which water will initially enter the soil */
      /* (infil_rate) */

      /* corrected 9/11/00 - mdh */
/*      if ((soiltavg[0] < FRZSOIL) && ((theta - thetas_bd[0]) < 0.13)) { */
      if ((soiltavg[0] < FRZSOIL) && ((thetas_bd[0] - theta) < 0.13)) {
        infil_rate = min(MIN_FRZN_COND, input_intensity);
        *impedance = 1;
      } else {
        if (input_intensity > infil_capacity) {
          infil_rate = infil_capacity;
        } else {
          infil_rate = input_intensity;
        }
      }

      /* fill each layer to saturation until water inputs are used up, or */
      /* time is up */

      while ((watrleft > 0.0) && (ilyr < numlyrs) &&
             (*cumtime < infil_time*SEC_PER_HOUR)) {

        theta = (float)swc[ilyr]/width[ilyr];
        wtosat = thetas_bd[ilyr]*width[ilyr] - swc[ilyr];
/*        if ((soiltavg[ilyr] < FRZSOIL) &&
           ((theta - thetas_bd[ilyr]) < 0.13)) { */
        /* corrected 9/11/00 - mdh */
        if ((soiltavg[ilyr] < FRZSOIL) &&
           ((thetas_bd[ilyr] - theta) < 0.13)) {
          infil_rate = min(MIN_FRZN_COND,infil_rate);
          *impedance = 1;
        } else if (infil_rate > satcond[ilyr]) {
          /* The layer can not absorb water at infiltration rate. */
          /* infiltration is now limited by this layer */
          infil_rate = satcond[ilyr];
        }

/*        printf("lyr=%1d\twtosat=%9.5f\tinfil_rate=%12.10f\n", ilyr, wtosat,
               infil_rate); */

        /* if the amount of water needed to bring the layer to saturation */
        /* is greater than the amount left to infiltrate... */

        if (wtosat > watrleft) {
          dt = watrleft/infil_rate;
          if ((*cumtime+dt) > (infil_time*SEC_PER_HOUR)) {
            dt = infil_time*SEC_PER_HOUR - *cumtime;
          }
          watrin = dt*infil_rate;
          watrleft -= watrin;
        } else {

          /* if the amount of water needed to bring the layer to saturation */
          /* is less than the amount left to infiltrate... */

          dt = wtosat/infil_rate;
          if ((*cumtime+dt) > infil_time*SEC_PER_HOUR) {
            dt = infil_time*SEC_PER_HOUR - *cumtime;
          }
          watrin = dt*infil_rate;
          watrleft -= watrin;
        }

/*        if (input_intensity > infil_rate) {
          *runoff += (input_intensity - infil_rate)*dt;
        } */

        *cumtime += dt;
        *cuminfl += watrin;
        swc[ilyr] += watrin;

        /* Re-calculate impedance based on new soil water content of layer */
        /* -cindyk 9/18/00 */
        theta = (float)swc[ilyr]/width[ilyr];
        if ((soiltavg[ilyr] < FRZSOIL) &&
           ((thetas_bd[ilyr] - theta) < 0.13)) {
          *impedance = 1;
        }

        if (debug) {
          printf("dt=%5.2f\twatrin=%12.10f\twatrleft=%12.10f\tcumtime=%5.2f",
                 dt, watrin, watrleft, *cumtime);
          printf("\tcuminfl=%12.10f\trunoff=%12.10f\n", *cuminfl, *runoff);
        }

        ilyr++;
      }  /* end while */

      tdiff = *cumtime - infil_time*SEC_PER_HOUR;

      *runoff = watrinput - *cuminfl;

/*      printf("runoff = %12.10f\n", *runoff); */

      wdiff = *cuminfl + *runoff - watrinput;

      /* Error checking */

      if (fabs(wdiff) > 0.00001) {
        fprintf(stderr, "Error in water balance:  cuminfl=%12.10f", *cuminfl);
        fprintf(stderr, "\trunoff=%12.10f\twatrinput=%12.10f\n", *runoff, watrinput);
        abortmssg("water balance failure in rainflux; inflow + runoff > water input");
      } else if (fabs(tdiff) > 0.005) { /* 0.001 */
        if (ilyr != numlyrs) {
          printf("rain accumulation time does not balance: seconds_rain=%12d", infil_time*SEC_PER_HOUR);
          printf("\tcumtime=%12.10f\n", *cumtime);
          printf("tdiff = %12.10f\n", tdiff);
          abortmssg("precipitation input time does not equal seconds_rain");
        }
      }

      /* Reset cumtime in case of floating point operations have caused it */
      /* to drift */
      *cumtime = infil_time*SEC_PER_HOUR;

/*      if (watrleft > 0.0) {
        printf("WATER LEFT = %12.10f,  RUNOFF = %12.10f\n", watrleft,*runoff);
      } */

      /* Now that all water has been added, drain each layer to field */
      /* capacity if there is no layer (frozen) impeding the flow. */
      /* -mdh 7/16/96 */

/*      if (!impedance) {
        hwdrain(swc, drain_out, numlyrs, swcfc, wfluxout, watertable,
                thetas_bd, width);
      } */

      return;
    }
