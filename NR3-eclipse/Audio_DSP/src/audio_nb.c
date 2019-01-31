#include "arm_math.h"
#include "audio_nb.h"
#include "audio_nr.h"

#define MAXIMP			20

/*

SNBA create_snba (int run, float32_t* in, float32_t* out, int inrate, int internalrate, int bsize, int ovrlp, int xsize,
	int asize, int npasses, float32_t k1, float32_t k2, int b, int pre, int post, float32_t pmultmin, float32_t out_low_cut, float32_t out_high_cut)
{
	SNBA d = (SNBA) malloc0 (sizeof (snba));
	d->run = run;
	d->in = in;
	d->out = out;
	d->inrate = inrate;
	d->internalrate = internalrate;
	d->bsize = bsize;
	d->ovrlp = ovrlp;
	d->xsize = xsize;
	d->exec.asize = asize;
	d->exec.npasses = npasses;
	d->sdet.k1 = k1;
	d->sdet.k2 = k2;
	d->sdet.b = b;
	d->sdet.pre = pre;
	d->sdet.post = post;
	d->scan.pmultmin = pmultmin;
	d->out_low_cut = out_low_cut;
	d->out_high_cut = out_high_cut;

	calc_snba (d);

	

	d->sdet.vp      = (float32_t *) malloc0 (d->xsize * sizeof (float32_t));
	d->sdet.vpwr    = (float32_t *) malloc0 (d->xsize * sizeof (float32_t));
	
	return d;
}

*/



void ATAc0 (int32_t n, int32_t nr, float32_t* A, float32_t* r)
{
    int i, j;
	memset(r, 0, n * sizeof (float32_t));
    for (i = 0; i < n; i++)
        for (j = 0; j < nr; j++)
            r[i] += A[j * n + i] * A[j * n + 0];
}

void multA1TA2(float32_t* a1, float32_t* a2, int32_t m, int32_t n, int32_t q, float32_t* c)
{
	int i, j, k;
    int p = q - m;
	memset (c, 0, m * n * sizeof (float32_t));              
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            int kmin = 0, kmax = 0;
            if (j < p)
            {
                for (k = i; k <= fmin(i + p, j); k++)
                    c[i * n + j] += a1[k * m + i] * a2[k * n + j];
            }
            if (j >= n - p)
            {
                for (k = fmax(i, q - (n - j)); k <= i + p; k++)
                    c[i * n + j] += a1[k * m + i] * a2[k * n + j];
            }
        }
    }
}

void multXKE(float32_t* a, float32_t* xk, int32_t m, int32_t q, int32_t p, float32_t* vout)
{
    int i, k;
	memset (vout, 0, m * sizeof (float32_t));
    for (i = 0; i < m; i++)
    {
        for (k = i; k < p; k++)
            vout[i] += a[i * q + k] * xk[k];
        for (k = q - p; k <= q - m + i; k++)
            vout[i] += a[i * q + k] * xk[k];
    }
}

void multAv(float32_t* a, float32_t* v, int32_t m, int32_t q, float32_t* vout)
{
	int i, k;
	memset (vout, 0, m * sizeof (float32_t));
    for (i = 0; i < m; i++)
    {
        for (k = 0; k < q; k++)
            vout[i] += a[i * q + k] * v[k];
    }
}

void xHat(int32_t xusize, int32_t asize, float32_t* xk, float32_t* a, float32_t* xout)
{
    int32_t i, j, k;
	int32_t a1rows = xusize + asize;
	int32_t a2cols = xusize + 2 * asize;
	
	float32_t r[xusize];
	float32_t ATAI[xusize * xusize];
	float32_t A1[a1rows * xusize];
	float32_t A2[a1rows * a2cols];
	float32_t P1[xusize * a2cols];
	float32_t P2[xusize];
	memset(r,0,xusize * sizeof(float32_t));
	memset(ATAI,0,xusize * xusize * sizeof(float32_t));
	memset(A1,0,a1rows * xusize * sizeof(float32_t));
	memset(A2,0,a1rows * a2cols * sizeof(float32_t));
	memset(P1,0,xusize * a2cols * sizeof(float32_t));
	memset(P2,0,xusize * sizeof(float32_t));
	
	
    for (i = 0; i < xusize; i++)
    {
        A1[i * xusize + i] = 1.0;
        k = i + 1;
        for (j = k; j < k + asize; j++)
            A1[j * xusize + i] = - a[j - k];
    }

    for (i = 0; i < asize; i++)
        {
            for (k = asize - i - 1, j = 0; k < asize; k++, j++)
                A2[j * a2cols + i] = a[k]; 
        }
    for (i = asize + xusize; i < 2 * asize + xusize; i++)
        {
            A2[(i - asize) * a2cols + i] = - 1.0;
            for (j = i - asize + 1, k = 0; j < xusize + asize; j++, k++)
                A2[j * a2cols + i] = a[k];
        }

    ATAc0(xusize, xusize + asize, A1, r);
    trI(xusize, r, ATAI);
    multA1TA2(A1, A2, xusize, 2 * asize + xusize, xusize + asize, P1);
    multXKE(P1, xk, xusize, xusize + 2 * asize, asize, P2);
    multAv(ATAI, P2, xusize, xusize, xout);

}

void invf(int32_t xsize, int32_t asize, float32_t* a, float32_t* x, float32_t* v)
{
    int i, j;
	memset (v, 0, xsize * sizeof (float32_t));
	for (i = asize; i < xsize - asize; i++)
	{
		for (j = 0; j < asize; j++)
			v[i] += a[j] * (x[i - 1 - j] + x[i + 1 + j]);
		v[i] = x[i] - 0.5 * v[i];
	}
	for (i = xsize - asize; i < xsize; i++)
	{
        for (j = 0; j < asize; j++)
            v[i] += a[j] * x[i - 1 - j];
        v[i] = x[i] - v[i];
    }
}

void det(int32_t Nsam, int32_t asize, float32_t* v, int32_t* detout, int32_t k1, int32_t k2)
{

    //const float32_t k1=8.0;  // some parameters for our detection algo
    //const float32_t k2=20.0;
    const int 	    b_max=10;//max 10 !!
    const int 	    imp_pre=2;
    const int 	    imp_post=2;


    int32_t i, j;
    float32_t medpwr, t1, t2;
    int32_t bstate, bcount, bsamp;
    float32_t vpwr[Nsam];
    float32_t vp[Nsam];


    //for (i = asize, j = 0; i < Nsam; i++, j++)
    for (i = asize; i < Nsam; i++)
	{
	    vpwr[i] = v[i] * v[i];
	//	vp[j] = vpwr[i];
	}
    for(i=0;i<Nsam-asize;i++)
      vp[i]=vpwr[i+asize];

    median(Nsam - asize, vp, &medpwr);

    t1 = (float32_t)k1 * medpwr;
    t2 = 0.0;
    for (i = asize; i < Nsam; i++)
    {
        if (vpwr[i] <= t1)
            t2 += vpwr[i];
        else if (vpwr[i] <= 2.0 * t1)
			t2 += 2.0 * t1 - vpwr[i];
    }
    t2 *= (float32_t)k2 / (float32_t)(Nsam - asize);
    for (i = asize; i < Nsam; i++)
    {
        if (vpwr[i] > t2)
            detout[i] = 1;
        else
            detout[i] = 0;
    }
    bstate = 0;
    bcount = 0;
    bsamp = 0;
    for (i = asize; i < Nsam; i++)
    {
        switch (bstate)
        {
            case 0:
                if (detout[i] == 1) bstate = 1;
                break;
            case 1:
                if (detout[i] == 0)
                {
                    bstate = 2;
                    bsamp = i;
                    bcount = 1;
                }
                break;
            case 2:
                ++bcount;
                if (bcount > b_max)
                    if (detout[i] == 1)
                        bstate = 1;
                    else
                        bstate = 0;
                else if (detout[i] == 1)
                {
                    for (j = bsamp; j < bsamp + bcount - 1; j++)
                        detout[j] = 1;
                    bstate = 1;
                }
                break;
        }
    }
    //maybe too tolerant!
/*    for (i = asize; i < (Nsam-1); i++) // remove all single detected impulses
      {
	if ((detout[i]==1) && (detout[i-1]==0) && (detout[i+1]==0))
	  detout[i]=0;
      }
*/
    for (i = asize; i < Nsam; i++)
    {
        if (detout[i] == 1)
        {
            for (j = i - 1; j > i - 1 - imp_pre; j--)
                if (j >= asize) detout[j] = 1;
        }
    }
    for (i = Nsam - 1; i >= asize; i--)
    {
        if (detout[i] == 1)
        {
            for (j = i + 1; j < i + 1 + imp_post; j++)
                if (j < Nsam) detout[j] = 1;
        }
    }
}

int scanFrame(int32_t xsize, int32_t pval, float32_t pmultmin, int32_t* det, int32_t* bimp, int32_t* limp,
            int32_t* befimp, int32_t* aftimp, int32_t* p_opt, int32_t* next)
{
    int32_t inflag = 0;
    int32_t i = 0, j = 0, k = 0;
    int32_t nimp = 0;
    float32_t td = 0.0;
    int32_t ti = 0.0;
	float32_t merit[MAXIMP] = { 0 };
	int32_t nextlist[MAXIMP];
	memset (befimp, 0, MAXIMP * sizeof (int32_t));
	memset (aftimp, 0, MAXIMP * sizeof (int32_t));
	memset (merit, 0,  MAXIMP * sizeof (float32_t));
	memset (nextlist, 0,  MAXIMP * sizeof (int32_t));

    while (i < xsize && nimp < MAXIMP)
    {
        if (det[i] == 1 && inflag == 0)
        {
            inflag = 1;
            bimp[nimp] = i;
            limp[nimp] = 1;
            nimp++;
        }
        else if (det[i] == 1)
        {
            limp[nimp - 1]++;
        }
        else
        {
            inflag = 0;
            befimp[nimp]++;
            if (nimp > 0)
                aftimp[nimp - 1]++;
        }
        i++;
    }
    for (i = 0; i < nimp; i++)
    {
        if (befimp[i] < aftimp[i])
            p_opt[i] = befimp[i];
        else
            p_opt[i] = aftimp[i];
        if (p_opt[i] > pval)
            p_opt[i] = pval;
        if ((p_opt[i] < (int32_t)(pmultmin * limp[i])))// || (limp[i]>20))   //|| > 20 ergänzt, um max Pulslänge einzuschr.
            p_opt[i] = -1;     //ist die zu korrigierende Zeitspanne zu groß, auf -1 setzen
    }
            
    for (i = 0; i < nimp; i++)
    {
        merit[i] = (float32_t)p_opt[i] / (float32_t)limp[i];
        nextlist[i] = i;
    }
    for (j = 0; j < nimp - 1; j++)
    {
        for (k = 0; k < nimp - j - 1; k++)
        {
            if (merit[k] < merit[k + 1])
            {
                td = merit[k];
                ti = nextlist[k];
                merit[k] = merit[k + 1];
                nextlist[k] = nextlist[k + 1];
                merit[k + 1] = td;
                nextlist[k + 1] = ti;
            }
        }
    }
    i = 1;
    if (nimp > 0)
        while (merit[i] == merit[0] && i < nimp) i++;
    for (j = 0; j < i - 1; j++)
    {
        for (k = 0; k < i - j - 1; k++)
        {
            if (limp[nextlist[k]] < limp[nextlist[k + 1]])
            {
                td = merit[k];
                ti = nextlist[k];
                merit[k] = merit[k + 1];
                nextlist[k] = nextlist[k + 1];
                merit[k + 1] = td;
                nextlist[k + 1] = ti;
            }
        }
    }
    *next = nextlist[0];
    return nimp;
}

void execFrame(float32_t* x_input, int32_t xsize, int32_t asize,int32_t npasses,float32_t pmultmin) //übergeordnete Funktion
{


	//const float32_t pmultmin 0.5;
	//float32_t x[xsize];
	//float32_t savex[xsize];
	//float32_t xHout[xsize];
	//float32_t exec_a[xsize];
	//float32_t exec_v[xsize];
		float32_t x[512];
		float32_t savex[512];
		float32_t xHout[512];
		float32_t exec_a[512];
		float32_t exec_v[512];



		int32_t i, k;
    int32_t pass;
    int32_t nimp;
	int32_t bimp[MAXIMP];
	int32_t limp[MAXIMP];
	int32_t befimp[MAXIMP];
	int32_t aftimp[MAXIMP];
	int32_t p_opt[MAXIMP];
    int32_t next = 0;
    int32_t p=0;
	//int32_t unfixed[xsize];
	//int32_t detout[xsize];
		int32_t unfixed[512];
		int32_t detout[512];


	for (int i=0; i<xsize;i++)
	  {
	    x[i]=0.0;
	    savex[i]=0.0;
	    xHout[i]=0.0;
	    exec_a[i]=0.0;
	    exec_v[i]=0.0;
	    unfixed[i]=0;
	    detout[i]=0;

	  }

	for (int i=0; i<MAXIMP;i++)
	  {
	    bimp[i]=0;
	    limp[i]=0;
	    befimp[i]=0;
	    aftimp[i]=0;
	    p_opt[i]=0;
	  }

	//******
	for (int i=0; i< xsize; i++) x[i] = x_input[2 * i];  //copy samples from FFT_Buffer , every second real sample
	//******

    memcpy (savex, x, xsize * sizeof (float32_t));
    asolve(xsize, asize, x, exec_a);
    invf(xsize, asize, exec_a, x, exec_v);
    det(xsize, asize, exec_v, detout,NR3.ka1,NR3.ka2);        // bis hierhin werden zunächst die Impulse detektiert
    for (i = 0; i < xsize; i++)
    {
        if (detout[i] != 0)
           x[i] = 0.0;
    }
    nimp = scanFrame(xsize,asize, pmultmin, detout, bimp, limp, befimp, aftimp, p_opt, &next); // wie liegen die Impulse, Abstand davor und dahinter,...
    for (pass = 0; pass < npasses; pass++)
    {
	memcpy (unfixed, detout, xsize * sizeof (int32_t));

	int32_t a_already_solved = 0;

	for (k = 0; k < nimp; k++)
        {
            if (k > 0)
                scanFrame(xsize, asize, pmultmin, unfixed, bimp, limp, befimp, aftimp, p_opt, &next);

            if ((p = p_opt[next]) > 0)
            {      
               // if (a_already_solved == 0)
                //  {
                    asolve(xsize, p, x, exec_a);
                //    a_already_solved = 1;  // do asolve only once per frame!
                //  }


        	xHat(limp[next], p, &x[bimp[next] - p], exec_a, xHout);

        	memcpy (&x[bimp[next]], xHout, limp[next] * sizeof (float32_t));


        	  memset (&unfixed[bimp[next]], 0, limp[next] * sizeof (int32_t));
            }
            else
            {
				memcpy (&x[bimp[next]], &savex[bimp[next]], limp[next] * sizeof (float32_t));
            }
        }
    }
    for (int i=0; i< xsize; i++) x_input[2 * i] = x[i];

}










void dR (int32_t n, float32_t* r, float32_t* y)
{
	int i, j, k;
    float32_t alpha, beta, gamma;
    float32_t z[n];
    y[0] = -r[1];
    alpha = -r[1];
    beta = 1.0;

    for(i=0;i<n;i++) z[n]=0.0;

    for (k = 0; k < n - 1; k++)
    {
        beta *= 1.0 - alpha * alpha;
        gamma = 0.0;
        for (i = k + 1, j = 0; i > 0; i--, j++)
            gamma += r[i] * y[j];
        alpha = - (r[k + 2] + gamma) / beta;
        for (i = 0, j = k; i <= k; i++, j--)
            z[i] = y[i] + alpha * y[j];
		memcpy (y, z, (k + 1) * sizeof (float32_t));
        y[k + 1] = alpha;
    }
	
}


void trI (int32_t n, float32_t* r, float32_t* B)
{
    int i, j, ni, nj;
    float32_t gamma, t, scale, b;
	float32_t y [n];
	float32_t v [n];

	memset(y,0,n * sizeof(float32_t));
	memset(v,0,n * sizeof(float32_t));

	scale = 1.0 / r[0];
    for (i = 0; i < n; i++)
        r[i] *= scale;
    dR(n - 1, r, y);

    t = 0.0;
    for (i = 0; i < n - 1; i++)
        t += r[i + 1] * y[i];
    gamma = 1.0 / (1.0 + t);
    for (i = 0, j = n - 2; i < n - 1; i++, j--)
        v[i] = gamma * y[j];
    B[0] = gamma;
    for (i = 1, j = n - 2; i < n; i++, j--)
        B[i] = v[j];
    for (i = 1; i <= (n - 1) / 2; i++)
        for (j = i; j < n - i; j++)
            B[i * n + j] = B[(i - 1) * n + (j - 1)] + (v[n - j - 1] * v[n - i - 1] - v[i - 1] * v[j - 1]) / gamma;
    for (i = 0; i <= (n - 1)/2; i++)
        for (j = i; j < n - i; j++)
        {
            b = B[i * n + j] *= scale;
            B[j * n + i] = b;
            ni = n - i - 1;
            nj = n - j - 1;
            B[ni * n + nj] = b;
            B[nj * n + ni] = b;
        }
	
}

void asolve(int32_t xsize, int32_t asize, float32_t* x, float32_t* a)
{
    int i, j, k;
    float32_t beta, alpha, t;
    float32_t r[asize + 1];
    float32_t z[asize +1];

    for (i = 0; i <= asize+1; i++)
      {
	r[i]=0.0;
	z[i]=0.0;
      }

    for (i = 0; i <= asize; i++)
    {
		for (j = 0; j < xsize; j++)
			r[i] += x[j] * x[j - i];
    }
    z[0] = 1.0;
    beta = r[0];
    for (k = 0; k < asize; k++)
    {
        alpha = 0.0;
        for (j = 0; j <= k; j++)
            alpha -= z[j] * r[k + 1 - j];
        alpha /= beta;
        for (i = 0; i <= (k + 1) / 2; i++)
        {
            t = z[k + 1 - i] + alpha * z[i];
            z[i] = z[i] + alpha * z[k + 1 - i];
            z[k + 1 - i] = t;
        }
        beta *= 1.0 - alpha * alpha;
    }
    for (i = 0; i < asize; i++)
	{
        a[i] = - z[i + 1];
		if (a[i] != a[i]) a[i] = 0.0;
	}

}

void median (int32_t n, float32_t* a, float32_t* med)
{
    int S0, S1, i, j, m, k;
    float32_t x, t;
    S0 = 0;
    S1 = n - 1;
    k = n / 2;
    while (S1 > S0 + 1)
    {
        m = (S0 + S1) / 2;
        t = a[m];
        a[m] = a[S0 + 1];
        a[S0 + 1] = t;
        if (a[S0] > a[S1])
        {
            t = a[S0];
            a[S0] = a[S1];
            a[S1] = t;
        }
        if (a[S0 + 1] > a[S1])
        {
            t = a[S0 + 1];
            a[S0 + 1] = a[S1];
            a[S1] = t;
        }
        if (a[S0] > a[S0 + 1])
        {
            t = a[S0];
            a[S0] = a[S0 + 1];
            a[S0 + 1] = t;
        }
        i = S0 + 1;
        j = S1;
        x = a[S0 + 1];
		do i++; while (a[i] < x);
        do j--; while (a[j] > x);
        while (j >= i)
        {
            t = a[i];
            a[i] = a[j];
            a[j] = t;
			do i++; while (a[i] < x);
            do j--; while (a[j] > x);
        }
        a[S0 + 1] = a[j];
        a[j] = x;
        if (j >= k) S1 = j - 1;
        if (j <= k) S0 = i;
    }
    if (S1 == S0 + 1 && a[S1] < a[S0])
    {
        t = a[S0];
        a[S0] = a[S1];
        a[S1] = t;
    }
	*med = a[k];
}



void lpc_calc(float32_t* wb,int32_t ns,int32_t ord, float32_t* lpcs)
{


      float32_t R[ord+1];  // takes the autocorrelation results
      float32_t k,alfa,s;

      float32_t any[ord+1];  //some internal buffers for the levinson durben algorithm
  // calculate the autocorrelation of insamp (moving by max. of #order# samples)
  for(int i=0; i < (ord+1); i++)
  {

	arm_dot_prod_f32(&wb[0],&wb[i],ns-i,&R[i]); // R is carrying the crosscorrelations
  }
  // end of autocorrelation



  //alternative levinson durben algorithm to calculate the lpc coefficients from the crosscorrelation

  R[0] = R[0] * (1.0 + 1.0e-9);

  lpcs[0] = 1;   //set lpc 0 to 1

  for (int i=1; i < ord+1; i++)
      lpcs[i]=0;                      // fill rest of array with zeros - could be done by memfill

  alfa = R[0];

  for (int m = 1; m <= ord; m++)
  {
      s = 0.0;
      for (int u = 1; u < m; u++)
          s = s + lpcs[u] * R[m-u];

      k = -(R[m] + s) / alfa;

      for (int v = 1;v < m; v++)
          any[v] = lpcs[v] + k * lpcs[m-v];

      for (int w = 1; w < m; w++)
          lpcs[w] = any[w];

      lpcs[m] = k;
      alfa = alfa * (1 - k * k);
  }

  // end of levinson durben algorithm

}
