
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      diffusiv.c
**
**  FUNCTION:  float diffusiv()
**
**  PURPOSE:   For estimating normalized diffusivity in soils.  Method of
**             Millington and Shearer (1971) Soil Science Literature Source:
**             Davidson, E.A. and S.E. Trumbore (1995).
**             Contributed by Chris Potter, NASA Ames
**
**  INPUTS:
**    A       - is the fraction of soil bed volume occuppied by field capacity
**              (intra-aggregate pore space), units 0-1
**    bulkden - bulk density (g/cm3)
**    wfps    - water-filled pore space (fraction 0.0-1.0)
**              (fraction of a porespace that is filled with water)
**              volumetric water / porosity
**
**  GLOBAL VARIABLES:
**    None
**
**  LOCAL VARIABLES:
**    debug      - flag to set debugging mode, 0 = off, 1 = on
**    my_theta_V - testing alternate way of computing the volumetric water
**                 content of the soil bed volume
**    PARTDENS   - particle density (g/cm3)
**    pfc        - water content of soil as a fraction of field capacity
**    porosity   - the fraction of soil bed volume occupied by pore space (P)
**                 (A + inter-aggregate pore space), units 0-1
**                  = 1 - bulkden/particle density
**    sw_p       - the fractional liquid saturation of the P component of
**                 total pore volume
**    s_wat      - the fractional liquid saturation of the A component of
**                 total pore volume
**    theta_A    - the volume of water per unit bed volume contained in
**                 intra-aggregate pore space
**    theta_P    - the volume of water per unit bed volume contained in
**                 inter-aggregate pore space
**    theta_V    - the volumetric water content of the soil bed volume
**    tp1 .. tp8 - intermediate variables
**
**  OUTPUT:
**    dDO - the normalized diffusivity in aggregate soil media, units 0-1
**
**  CALLED BY:
**    dailymoist()
**
**  CALLS:
**    None
**
*****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mssg.h"

#define PARTDENS 2.65           /* Particle Density (g/cm3) */
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
    float diffusiv(float *A, float *bulkden, float *wfps)
    {
      float porosity;
      float theta_A, theta_P, theta_V, my_theta_V;
      float tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8;
      float s_wat, sw_p;
      float pfc;
      float dDO;
      int   debug =0;

      porosity = 1.0f - (*bulkden)/(float)PARTDENS;
      if ((porosity - *A) <= 0.0) {
        abortmssg("Mis-match between bulk density and field capacity in soils.in");
      }

      pfc = (*wfps) /((*A)/porosity);

      /* combined vfrac and theta_v if blocks. They were both dependent on pfc < 1.0 */
      if (pfc >= 1.00) {
        /* vfrac = ((*wfps)*porosity - *A) / (porosity - (*A));
           Removed the divide by (porosity - (*A) times (porosity - (*A) for pfc >1
           theta_V = (*A) + (min(vfrac,1.0)*(porosity - (*A)));
        */
        theta_V = min((*wfps)*porosity, porosity); /*(*A) + (min((*wfps)*porosity - *A, porosity - (*A))); */
      } else {
        theta_V = *wfps * porosity; /* pfc * *A  = *wfps /(*A/porosity) * *A */
      }

      my_theta_V = *wfps * (1.0f - *bulkden/(float)PARTDENS);

      theta_P = (theta_V < (*A)) ? 0.0f : theta_V - (*A);
      theta_A = (theta_V > (*A)) ? (*A) : theta_V;

      s_wat = min(1.0f, theta_V/(*A));
      sw_p  = min(1.0f, theta_P/(porosity - *A));

      if (debug) {
        printf("In diffusiv, *A           = %f\n", *A);
        printf("In diffusiv, *bulkden     = %f\n", *bulkden);
        printf("In diffusiv, porosity     = %f\n", porosity);
        printf("In diffusiv, wfps         = %f\n", *wfps);
        printf("In diffusiv, porosity - A = %f\n", (porosity - *A));
        printf("In diffusiv, pfc          = %f\n", pfc);
        printf("In diffusiv, theta_V      = %f\n", theta_V);
        printf("In diffusiv, my_theta_V   = %f\n", my_theta_V);
        printf("In diffusiv, theta_P      = %f\n", theta_P);
        printf("In diffusiv, theta_A      = %f\n", theta_A);

        printf("In diffusiv, s_wat        = %f\n", s_wat);
        printf("In diffusiv, sw_p         = %f\n", sw_p);
      }

      tp1 = (float)pow((1.0 - (double)s_wat), 2.0);
      tp2 = ((*A) - theta_A) / (*A + (1.0f - porosity));
      tp3 = (float)pow((double)tp2,(0.5*(double)tp2 + 1.16));
      tp4 = (float)(1.0 - pow((double)((porosity-(*A))),
                              (0.5*(double)((porosity-(*A))) + 1.16)));
      tp5 = (porosity-(*A))-theta_P;
      if (tp5 > 0.0) {
        tp6 = (float)pow((double)tp5, (0.5*(double)tp5 + 1.16));
      } else {
        tp6 = 0.0f;
      }
      tp7 = (float)pow((1.0-(double)sw_p), 2.0);
      tp8 = max(0.0f, (tp1*tp3*tp4*(tp5-tp6)) /
                      (1.0E-6f + (tp1*tp3*tp4) + tp5 - tp6) * 1.0E7f);

      if (debug) printf("From diffusiv: %f %f %f %f %f %f %f %f\n",
                        tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8);
      dDO = max(0.0f, (tp8/1.0E7f + tp7*tp6));
      if (debug) {
	    printf("In diffusiv, dDO = %f\n", dDO);
      }

/*      dDO /= 0.20;
      dDO = min(dDO, 1.0); */

      return(dDO);
    }
