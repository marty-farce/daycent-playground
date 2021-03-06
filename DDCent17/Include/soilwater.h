/*****************************************************************************
**
**  FILE:    soilwater.h
**
**  AUTHOR:  Melannie Hartman  9/7/93 - 8/21/96
**
**  Updates:
**    WFPS effect on N2O flux                  KLK 7 Aug 13
**    buffer output for internal redirection   KLK
**
*****************************************************************************/

#include <stdio.h>
#include "swconst.h"
#include "sitsoil.h"

#define FILEPATH_MAX 60   /* Max # of chars in a file path-name */

#define INPTSTRLEN 140    /* Max length of input file line */
#define OTPTSTRLEN 80     /* Max length of output file string */

typedef struct {
  int verbose;
  int debug;
} FLAG_S, *FLAG_SPT;

typedef struct
{
  char wbname[FILEPATH_MAX];  /* Water Balance File */
  FILE *fp_outf;
  FILE *fp_wb;
  FILE *fp_soiln;
  FILE *fp_co2;
  FILE *fp_swc;
  FILE *fp_wfps;
  FILE *fp_soiltavg;
  FILE *fp_soiltmin;
  FILE *fp_soiltmax;
  FILE *fp_stempdx;
  FILE *fp_bio;
  FILE *fp_wflux;
  FILE *fp_mresp;
  FILE *fp_yearsum;
  FILE *fp_livec;
  FILE *fp_deadc;
  FILE *fp_soilc;
  FILE *fp_sysc;
  FILE *fp_tgmonth;
  FILE *fp_dN2lyr;
  FILE *fp_dN2Olyr;
  FILE *fp_gresp;
  FILE *fp_dels;
  FILE *fp_dcsip;
  FILE *fp_harv;
  FILE *fp_cflows;
  FILE *fp_yrcflows;
  FILE *fp_methane;
/*!!  FILE *fp_snow; */
  int  write_wb;
  int  write_co2;
  int  write_soiln;
  int  write_swc;
  int  write_wfps;
  int  write_soiltavg;
  int  write_soiltmin;
  int  write_soiltmax;
  int  write_stempdx;
  int  write_bio;
  int  write_wflux;
  int  write_mresp;
  int  write_yearsum;
  int  write_livec;
  int  write_deadc;
  int  write_soilc;
  int  write_sysc;
  int  write_tgmonth;
  int  write_dN2lyr;
  int  write_dN2Olyr;
  int  write_gresp;
  int  write_dels;
  int  write_dcsip;
  int  write_harvest;
  int  write_cflows;
  int  write_yrcflows;
  int  write_methane;
} FILES_S, *FILES_SPT;

typedef struct
{
  float stmtemp[MAXSTLYR];
  float soiltavg[MXSWLYR];
  float soiltmin[MXSWLYR];
  float soiltmax[MXSWLYR];
} SOIL_S, *SOIL_SPT;

float c_shwave(int month, float rlatitude, int jday);

/*void calcdefac(int *texture, float *stemp, float *tfunc, float *bgwfunc,
               float *agdefac, float *bgdefac, float *avgwfps, float teff[],
               float *rprpet, int *idef);

void calcpet(int *jday, int *month, float *tmin, float *tmax, float *tavg,
             float *solrad, float *rhumid, float *windsp, float *snow,
             int *petswitch, float fwloss[4], float *sitlat, float *petdly);

void daylen(int *curday, float *sitlat, float *dylngth);*/

void fracbslos(float *fbse, float *fbst, float blivelai);

void getdiff(float *valsubt, double valtochk, double valmin);

void h2oflux(int jday, int numlyrs, double swc[MXSWLYR], double swcmin[MXSWLYR],
             float minpot[MXSWLYR], float depth[MXSWLYR], float width[MXSWLYR],
             float satcond[MXSWLYR],  LAYERPAR_SPT layers,
             float soiltavg[MXSWLYR], float watrinput, float bserate,
             float *soilEvap, float hours_rain_param, float dmpflux,
             float *aet, double *inipswc, float *outflow,
             float wfluxout[MXSWLYR], float snowpack, float *runoffdly,
             float *basef, float *baseflow, int watertable, float hpotdeep,
             float ksatdeep);

void hwdrain(double swc[MXSWLYR], double *drain_out, int numlyrs,
             float swcfc[MXSWLYR], float wfluxout[MXSWLYR], int watertable,
             float thetas_bd[MXSWLYR], float width[MXSWLYR]);

void initdaily(int month, float biolive, float biodead, float blitter,
               float *biomass, float *blivelai, float *vegcov, float *totagb,
               LAYERPAR_SPT layers);

/*void initlyrs(char *soilname, SITEPAR_SPT sitepar, LAYERPAR_SPT layers,
                  FLAG_SPT flags);
void initsite(char *sitename, SITEPAR_SPT sitepar, LAYERPAR_SPT layers,
                  FLAG_SPT flags);
*/
void initlyrs (SITEPAR_SPT sitepar, LAYERPAR_SPT layers, FLAG_SPT flags);
void inputlyrs(SITEPAR_SPT sitepar, LAYERPAR_SPT layers, FLAG_SPT flags);
void initsite(SITEPAR_SPT sitepar, LAYERPAR_SPT layers, FLAG_SPT flags);

void initsrad(double elevation, double latitude, double aspect, double slope,
              double ehoriz, double whoriz, float daylength[NDAY]);

/*void initsw(float *sitlat, float swcinit[MXSWLYR], int *nlayer,
                 float daylength[NDAY], float rwcf[CMXLYR], float adep[CMXLYR]);
*/

void litstcr_evap(float *cwlit, float *cwstcr, float *petleft, float *aet,
                  float totlit, float totstcr);

float petrad(int jday, float rlatitude,
                 float windsp, float rhumid, float avgtemp, float clrsky,
                 float reflec, float solrad, float albedo, float snowpack);

float pevapdly(float tmin, float tmax, float sitlat, int jday, int month);

void potbse(float *bserate, float totagb, float fbse, float petday,
            LAYERPAR_SPT layers, float biolive);

void potbst(float *bstrate, float swpavg, float biolive, float biodead,
            float fbst, float petday, float co2val);

void pteevap(float *bserate, float *bstrate, float petleft);

void rainflux(float watrinput, int infil_time, double swc[MXSWLYR],
              float thetas_bd[MXSWLYR], float width[MXSWLYR],
              float satcond[MXSWLYR], float swcfc[MXSWLYR], float incap,
              float soiltavg[MXSWLYR], int numlyrs, double *runoff,
              double *cumtime, double *cuminfl, double *drain,
              float wfluxout[MXSWLYR], int *impedence);

void setamov(float amovdly[CMXLYR], int nlayer, float wfluxout[MXSWLYR],
             int numlyrs, int lbnd[CMXLYR]);

void setasmos(float asmos[CMXLYR], int *nlayer, double swc[MXSWLYR],
              int *numlyrs, float avh2o[3], float rwcf[CMXLYR]);

void showlyrs(double swc[MXSWLYR], int numlyrs);

void soiltransp(float transp[MXSWLYR], int numlyrs, float bstrate,
                LAYERPAR_SPT layers, float *aet);

void snowmodel(int jday, float tmax, float tmin, float tavg, float tmpcrit,
               float ppt, float *pptadj, float netrad, float relhum,
               float windsp, float clrsky, float rlatitude, float albedo,
               float *snow, float *Emelt, float *accum, float *Esublim,
               float *petlocal, float sublimscale, float petday,
               float tmelt[2], float *snlq, double srad, float daylength);

void snowCent(float tmelt[2], float tave, float pptactual, float *pptsoil,
              float *snow, float *snlq, float *petleft, float *melt,
              float *accum, float *sublim, float tmin, float tmax,
              double srad, float daylength);

float svapor(float atemp);

float swpotentl(double swc, int ilyr, LAYERPAR_SPT layers, char callname[]);

float tanfunc(float z, float a, float b, float c, float d);

float trwtavg(int *nlaypg, LAYERPAR_SPT layers, FLAG_SPT flags);

float watrate(float swp, float petday, float a, float b);

/*void watrbal(int *jdy, float *time, float *ppt, float *accum, float *melt,
                 double *deltaswc, float *evap, float *transp,
                 float *sublim, float *intrcpt, float *outflow, float *snlq1,
                 float *snlq2, float *snow, float *runoffdly);*/

void watreqn(float sand, float clay, float *thetas, float *psis, float *b);

/*
void watrflow(int *jday, int *month, int *nlayer, int *nlaypg,
                  int *watertable, int *watrflag,
                  float *avgtemp, float *tempmin, float *tempmax,
                  float *solrad, float *rhumid, float *windsp, float *pptactual,
                  float *biolive, float *blitter, float *biodead,
                  float rwcf[CMXLYR], float avh2o[3],
                  float asmos[CMXLYR], float *snowpack, float *snlq,
                  float amovdly[CMXLYR], float *pet, float *evaptot,
                  float *trantot, float *stream1, float *basef,
                  float *pottransp, float *baseflow, float *accum,
                  float *melt, float *intrcpt, float *outflow, float tmelt[],
                  float *sublim, float wfluxout[], float *time, float *strplt,
                  float *co2val, float *tmns, float *tmxs, float *runoffdly,
                  float *tadep, float *soiltavewk, float *daylength,
                  float *woodb, float *elitst, float *pmxtmp, float *pmntmp,
                  float *pmxbio, float *srfctemp, float *stsys,
                  float *ststart, float *stamt, float *litrcrbn,
                  float *watr2sat, double *srad, float *sradadj);
*/

void watrlit(float watrinput, float *pptsoil, float *wintlit, float blitter);

void watrstcr(float *pptleft, float *wintstcr, float ppt, float vegcov);

void wfps(LAYERPAR_SPT layers);

float wfunc_pulse(float *ppt, float *snow);

/*void wrtsoiln(float *time, int *jday, double *ammonium, double nitrate[]);

void wrtco2(float *time, int *jday, float co2PPM[]);*/

void wrtswc(FILE *fp, float time, int jday, double swc[], float width[],
            int numlyrs);

void wrtstemp(FILE *fp, float time, int jday, float soiltavg[], int numlyrs);

void wrtwfps(FILE *fp, float time, int jday, float wfps[], int numlyrs,
             float width[]);

void soiltemp(int jday, float tmin, float tmax, float depth[MXSWLYR],
              float width[MXSWLYR], float fieldc[MXSWLYR], float sand[MXSWLYR],
              float clay[MXSWLYR], float org[MXSWLYR], float lyblkd[MXSWLYR],
              double swc[MXSWLYR], int numlyrs, float soiltavg[MXSWLYR],
              float soiltmin[MXSWLYR], float soiltmax[MXSWLYR],
              float stmtemp[MAXSTLYR], float tmns, float tmxs,
              float *soiltavewk, float srfctemp, float diurnal_range);

void surftemp(float elitst, float pmxtmp, float pmntmp, float pmxbio,
              float tempmax, float tempmin, float *tmxs, float *tmns,
              float *srfctemp, float daylength, float biolive, float biodead,
              float blitter, float woodb, float stsys, float ststart,
              float stamt, float time, float snowpack, float *diurnal_range,
              float litrcrbn, int SnowFlag);

/*void wrtbio(float *time, int *curday, float *aglivc, float *bglivc,
            float *aglivn, float *bglivn, float *rleavc, float *frootc,
            float *fbrchc, float *rlwodc, float *crootc);*/
