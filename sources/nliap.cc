//#
//# FILE: nliap.cc -- Normal form Liapunov module
//#
//# $Id$
//#

#define MAXIT 10
#define TOL (T)(.00000001)

#include <math.h>
#include "nliap.h"
#include "gfunct.h"

//-------------------------------------------------------------------------
//                     NFLiapParams: Member functions
//-------------------------------------------------------------------------

NFLiapParams::NFLiapParams(gStatus &status_)
  : LiapParams(status_)
{ }

//-------------------------------------------------------------------------
//                      NFLiapFunc<T>: Class definition
//-------------------------------------------------------------------------

template <class T>
class NFLiapFunc : public LiapFunc<T>, public gBC2FunctMin<T>   {
  private:
    int niters, nevals;
    const Nfg<T> &N;
    MixedProfile<T> p, pp;
//    NFLiapParams<T> &params;

    T Value(const gVector<T> &x);
    int Deriv(const gVector<T> &p, gVector<T> &d);
    int Hess(const gVector<T> &p, gMatrix<T> &d);

    T LiapDerivValue(int i, int j, const MixedProfile<T> &p) const;

  public:
    NFLiapFunc(const Nfg<T> &NF, const LiapParams &P, 
	       const MixedProfile<T> &s); 
    virtual ~NFLiapFunc();

    void Randomize(void);
    int Optimize(int &iter, T &value);
    void Output(gOutput &f) const;

    long NumIters(void) const;
    long NumEvals(void) const;

    const MixedProfile<T> &GetProfile(void) const;
};

//------------------------------------------------------------------------
//               NFLiapFunc<T>: Constructor and destructor
//------------------------------------------------------------------------

template <class T>
NFLiapFunc<T>::NFLiapFunc(const Nfg<T> &NF, const LiapParams & LP,
			  const MixedProfile<T>& s)
  : gBC2FunctMin<T>(s.Length(),LP.status), niters(0), nevals(0),
    N(NF), p(s), pp(s)
{ }

template <class T> NFLiapFunc<T>::~NFLiapFunc()
{ }


template <class T> const MixedProfile<T> &NFLiapFunc<T>::GetProfile(void) const
{
  return pp;
}

template <class T> void NFLiapFunc<T>::Output(gOutput &f) const
{
  f << "\np = " << pp;
 }

template <class T> long NFLiapFunc<T>::NumIters(void) const
{
  return niters;
}

template <class T> long NFLiapFunc<T>::NumEvals(void) const
{
  return nevals;
}

template <class T> int NFLiapFunc<T>::Optimize(int &iter, T &value)
{
//  if (params.plev >= 3 && params.outfile)
//    *params.outfile << "\np= " << pp;
//  T val = (T) 0;
//  int iter = 0;
  return   DFP(pp, iter, value);
//  if (params.plev > 0 && params.outfile)
//    *params.outfile << "\np= " << pp << " f = " << val;
//  return (val < (T) ((T) 1 / (T) 100000));
}

template <class T> void NFLiapFunc<T>::Randomize(void)
{
  T sum, tmp;

  for (int i = 1; i <= N.NumPlayers(); i++)  {
    sum = (T) 0;
    int j;
    for (j = 1; j < pp.GetNFSupport().NumStrats(i); j++)  {
      do
	tmp = (T) Uniform();
      while (tmp + sum > (T) 1);
      pp(i,j) = tmp;
      sum += tmp;
    }
    pp(i,j) = (T) 1 - sum;
  }
}

template <class T> T NFLiapFunc<T>::Value(const gVector<T> &v)
{
  static const T BIG1 = (T) 100;
  static const T BIG2 = (T) 100;

  nevals++;

  ((gVector<T> &) p).operator=(v);
  MixedProfile<T> tmp(p);
  gPVector<T> payoff(p);
  T x, result((T) 0), avg, sum;
  payoff = (T) 0;

  result = (T) 0;
  for(int i = 1; i <= N.NumPlayers(); i++) {
    tmp.CopyRow(i, payoff);
    avg = sum = (T) 0;
	// then for each strategy for that player set it to 1 and evaluate
    int j;
    for (j = 1; j <= p.GetNFSupport().NumStrats(i); j++) {
      tmp(i, j) = (T) 1;
      x = p(i, j);
      payoff(i, j) = tmp.Payoff(i);
      avg += x * payoff(i, j);
      sum += x;
      if (x>(T)0) x=0;
      result += BIG1*x*x;         // add penalty for neg probabilities
      tmp(i,j) = (T) 0;
    }
    tmp.CopyRow(i, p);
    for(j=1;j<=p.GetNFSupport().NumStrats(i);j++) {
      x=payoff(i,j)-avg;
      if (x<=(T)0) x=(T)0;
      result += x*x;          // add penalty if not best response
    }
    x=sum - ((T) 1);
    result += BIG2*x*x ;          // add penalty for sum not equal to 1
  }
  return result;
}

template <class T> int NFLiapFunc<T>::Deriv(const gVector<T> &v, gVector<T> &d)
{
  ((gVector<T> &) p).operator=(v);
  int i1,j1,ii;
  T avg;
  
  for(i1=1,ii=1;i1<=N.NumPlayers();i1++) {
    avg=(T)0;
    for(j1=1;j1<=p.GetNFSupport().NumStrats(i1);j1++) {
      d[ii]=LiapDerivValue(i1,j1,p);
      avg+=d[ii];
      ii++;
    }
    avg/=(T)p.GetNFSupport().NumStrats(i1);
    ii-=p.GetNFSupport().NumStrats(i1);
    for(j1=1;j1<=p.GetNFSupport().NumStrats(i1);j1++) {
      d[ii]-=avg;
      ii++;
    }
  }
  return 1;
}

template <class T>
T NFLiapFunc<T>::LiapDerivValue(int i1, int j1, const MixedProfile<T> &p) const
{
  int i, j;
  T x, x1,psum;
  
  x=(T)0;
  for(i=1;i<=N.NumPlayers();i++) {
    psum=(T)0.0;
    for(j=1;j<=p.GetNFSupport().NumStrats(i);j++) {
      psum+=p(i,j);
      x1=p.Payoff(i,i,j)-p.Payoff(i);
      if(i1==i) {
	if(x1>(T)0)x-=x1*(p.Payoff(i,i1,j1));
      }
      else {
	if(x1>(T)0)x+=x1*(p.Payoff(i,i,j,i1,j1)-p.Payoff(i,i1,j1));
      }
    }
    if(i==i1)x+=psum-(T)1.0;
  }
  if(p(i1,j1)<(T)0.0)x+=p(i1,j1);
  return (T)2.0*x;
};

template <class T> int NFLiapFunc<T>::Hess(const gVector<T> &, gMatrix<T> &)
{
  return 1;
}


//------------------------------------------------------------------------
//                    NFLiapModule<T>: Member functions
//------------------------------------------------------------------------

template <class T>
NFLiapModule<T>::NFLiapModule(const Nfg<T> &N, NFLiapParams &p,
			      MixedProfile<T> &s)
  : LiapModule<T>(p), N(N), S(s)
{ }

template <class T> NFLiapModule<T>::~NFLiapModule()
{ }

template <class T>
const gList<MixedProfile<T> > &NFLiapModule<T>::GetSolutions(void) const
{
  return solutions;
}

template <class T> LiapFunc<T> *NFLiapModule<T>::CreateFunc(void)
{
//  return new NFLiapFunc<T>(nf, (NFLiapParams&) params);
  return new NFLiapFunc<T>(N, params, S);
}

template <class T>
void NFLiapModule<T>::AddSolution(const LiapFunc<T> *const F)
{
  solutions.Append(MixedProfile<T>(((NFLiapFunc<T> *) F)->GetProfile()));
}


#ifdef __GNUG__
#define TEMPLATE template
#elif defined __BORLANDC__
#define TEMPLATE
#pragma option -Jgd
#endif   // __GNUG__, __BORLANDC__

TEMPLATE class NFLiapModule<double>;
TEMPLATE class NFLiapFunc<double>;


