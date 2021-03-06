
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      showminrl.c
**
**  FUNCTION:  void showminrl()
**
**  PURPOSE:   This subroutine updates the ammonium and nitrate pools
**             whenever a soil minerl layer is updated.
**
**  INPUTS:
**     ammonium  - total ammonium in soil mineral pool (gN/m2)
**     minerl[]  - soil minerl pool, 1..nlayer (g/m2)
**                 minerl is a 2-D array in FORTRAN.  FORTRAN stores multi-
**                 dimensioned arrays in column-major order.  In this routine
**                 it is a 1-D array, where minerl(clyr,iel) in FORTRAN is
**                 equivalent to minerl[(iel-1)*CMXLYR + (clyr-1)] in this
**                 routine.
**     nitrate[] - total nitrate in soil mineral pool, distributed among soil
**                 water model layers (gN/m2)
**     nlayer    - number of layers in Century soil profile
**     subname   - name of calling routine, space delimited max 11 characters
**                 NO longer modified: copy to a local string to prevent termination from
**                 causing a memory crash KLK2018/08/18
**
**  GLOBAL VARIABLES:
**    CMXLYR     - maximum number of Century soil layers (10)
**    MXSWLYR    - maximum number of soil water model layers (21)
**
**  EXTERNAL VARIABLES:
**    layers          - soil water soil layer structure
**    layers->numlyrs - total number of layers in the soil water model soil
**                      profile
**
**  LOCAL VARIABLES:
**    clyr       - Century soil layer (1..nlayer)
**    iel        - current element, set to 0 for nitrogen
**    ilyr       - current layer in the soil profile
**    minerl_sum - minerl[], summed over soil layers
**    npool_sum  - nitrate[], summed over soil layers
**
**  OUTPUTS:
**    None
**
**  CALLED BY:
**    calciv
**
**  CALLS:
**    None
**
*****************************************************************************/

#include <math.h>
#include <stdio.h>
#include "soilwater.h"

    void showminrl(int *nlayer, float minerl[], double *ammonium,
                   double nitrate[], char *subname, long int strlen)
    {

      int    clyr, ilyr;
      int    iel = 0;    /* Nitrogen */
      double npool_sum;
      double minerl_sum;
      char callr[12] = {'\0'}; /* set array to initial all NUL bytes */

      extern LAYERPAR_SPT layers;

      npool_sum = 0.0;
      minerl_sum = 0.0;
      /* copy caller name to a local variable */
      ilyr = 0;
      while(subname[ilyr] != ' '  &&  ilyr <= 10) {callr[ilyr] = subname[ilyr]; ilyr++;}
      callr[ilyr] = '\0';

      /* Sum ammonium and nitrate pools and compare to sum of minerl pool */

      npool_sum = *ammonium;
      for (ilyr=0; ilyr < MXSWLYR; ilyr ++) {
        npool_sum += nitrate[ilyr];
      }

      for (clyr=0; clyr < CMXLYR; clyr++) {
        minerl_sum += minerl[(iel)*CMXLYR + (clyr)];
      }

      fprintf(stdout, "%s: npool_sum = %12.10f   minerl_sum = %12.10f  ",
              callr, npool_sum, minerl_sum);
      fprintf(stdout, "npool-minrl = %12.10f\n ", npool_sum - minerl_sum);

      for (clyr=0; clyr < *nlayer; clyr++) { /* was <=really shouldn't print beyond the array */
        fprintf(stdout, "minerl[%1d][N] = %12.10f\n", clyr+1,
                minerl[(iel)*CMXLYR + (clyr)]);
      }
      for (ilyr=0; ilyr < layers->numlyrs; ilyr ++) {/* was <=really shouldn't print beyond the array */
        fprintf(stdout, "nitrate[%1d] = %12.10f\n", ilyr, nitrate[ilyr]);
      }
      fprintf(stdout, "ammonium = %12.10f\n", *ammonium);

      return;
    }
