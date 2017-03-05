#include <mex.h> 
static int direct_gateway(char *fname,void F(void)) { F();return 0;};
extern Gatefunc mex_ra_scilab;
static GenericTable Tab[]={
  {mex_gateway,mex_ra_scilab,"ra_scilab"},
};
 
int C2F(librasch_sci)()
{
  Rhs = Max(0, Rhs);
  (*(Tab[Fin-1].f))(Tab[Fin-1].name,Tab[Fin-1].F);
  return 0;
}
