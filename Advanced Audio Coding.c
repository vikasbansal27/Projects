/*
This application program can be used to convert audio or any serial data in advanced audio coded format.
Length of the data entered should be a power of 2.
This application explores the data compression ability of Modified Discrete Cosine Transform (MDCT).
Data is first trnasformed to a frequency domain using MDCT. 
After that the coefficients obtained are quantized.
After quantization, we can apply psychoacoustic model to neglect frequencies which are not in human readable range.
Entropy coding is then performed on quantized data to obtain final compressed data. (Huffman code is used here).     
*/


#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

int n=16;               // Length of data stream.
int k;
float pi=4*atan(1);     // Value of pi
float dst1[3000];       
float dct1[3000];
float z[3000];
float mdctf[3000];
float mdct_final[3000];
void dct(float*);
void dst(float*);
void mdct(void);
void quant(void);
void IMDCT(void);
void prob(void);
int quan[3000];
int list[3000];
float IMDCT1[3000];
int length=0;
struct node{
	int key;
	float prob;
	struct node *left,*right,*parent,*next;
};


struct node *lsort(struct node *);
struct node *reduce(struct node *);
void display(struct node *temp);
void code(char *b, struct node *p);
int paw(int a,int i);
void huffman(void);

void print(struct node *p)     //Func to print the given linked list//
{
	while(p !=NULL)
	{
		//printf("%d %f -- ",p->key,p->prob);
		p=p->next;
	}
	printf("\n \n \n");
}


void swap(int *x,int *y)
{
   int temp;
   temp = *x;
   *x = *y;
   *y = temp;
}

int choose_pivot(int i,int j )
{
   return((i+j) /2);
}




void quicksort(int list[],int m,int n)
{
   int key,i,j,k;
   if( m < n)
   {
      k = choose_pivot(m,n);
      swap(&list[m],&list[k]);
      key = list[m];
      i = m+1;
      j = n;
      while(i <= j)
      {
         while((i <= n) && (list[i] <= key))
                i++;
         while((j >= m) && (list[j] > key))
                j--;
         if( i < j)
                swap(&list[i],&list[j]);
      }
	  // swap two elements
      swap(&list[m],&list[j]);
	  // recursively sort the lesser list
      quicksort(list,m,j-1);
      quicksort(list,j+1,n);
   }
}
void printlist(int list[],int n)
{
   int i;
  // for(i=0;i<n;i++)
//      printf("%d\t",list[i]);
}
struct {
	int key;
	float prob;
}typedef huff;


struct {
	float r;
	float c;
}typedef complex;
complex Sum(int, int, int,float*);
huff hm[3000];

int main()
{
	float data[3000]={1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
	int x_dash[3000],x_ddash[3000],x=0,m=0,m1=0,e=0,st=0;
	float a[3000]={0},b[3000]={0},rotations[3000]={0},temp;
	float pi = 4*atan(1);
	//printf("number?\n");
	//scanf("%d",&n);
	st=0;
	e=n-1;
	m=n/2-1;
	m1=n/2-1;
	for(x=0;x<n/2;x++)
	{
		x_dash[x]=data[st]-data[e];
		x_ddash[x]=data[st++]+data[e--];
		//printf("\n%d and %d\n",x_dash[x],x_ddash[x]);
	}
	st=0;
	for(x=0;x<n/4;x++)
	{
		rotations[x]= ((2*x+1)*pi)/(2*n);
	
		a[x]=(x_dash[st]-x_ddash[m])*cos(rotations[x])-(x_ddash[st]-x_dash[m])*sin(rotations[x]);
		b[x]=(x_dash[st]-x_ddash[m])*sin(rotations[x])+(x_ddash[st]-x_dash[m])*cos(rotations[x]);
		st++;
		m--;
	}
	printf("\n");
	for(x=0;x<n/4;x++)
	{
 	//	printf("%f  ",a[x] );	
	}
	
	printf("\n");
	for(x=0;x<n/4;x++)
	{
 	//	printf("\t%f ",b[x] );	
	}
	
	printf("\n\n");	

 dct(a);
 dst(b); 
 printf("\n\n");
 mdct();	
 quant();
 prob();
 huffman();
 IMDCT();
 
 return 0;
 }

void prob()
{
	int i = 0;
   for(i=0;i<n;i++)
   {
	   list[i]=quan[i];
	}	   
   printf("The list before sorting is:\n");
   printlist(list,n);
   
   // sort the list using quicksort
   quicksort(list,0,n-1);
	printf("\n\n");
   // print the result
   printf("The list after sorting using quicksort algorithm:\n");
   printlist(list,n);

   for(i=0;i<n;i++)
   {
   	hm[i].prob=0;
   }
   hm[0].key=list[0];
   hm[0].prob=1;
   printf("\n\n");
   int j=1;
   for(i=1,j=1;i<n,j<n;i++)
   {
	   if(list[i]==list[i-1])
	   {
	   	hm[j-1].prob++;
	   }
	   else
	   {
	      hm[j].key=list[i];
	   	hm[j].prob=1;
	  		j++;
	   }
	 }
	 for(i=0;i<n;i++)
	 {
	 	if(hm[i].key==list[n-1])
	  			length=i+1;
	 }
	 int total=0;
	 for(i=0;i<length;i++)
	 {
		 total+=hm[i].prob;
		 printf("\n\n");
		 printf("key-->%d \t prob-->%f",hm[i].key,hm[i].prob);
	}
	printf("\n\n");
	for(i=0;i<length;i++)
	{
		hm[i].prob/=total;
		 printf("\n\n");
		 printf("key-->%d \t prob-->%f",hm[i].key,hm[i].prob);
	}
	
}




void huffman(void)
{
	char ch,b[3000];
	b[0]='\0';

	struct node *h,*t;
	h=(struct node *)malloc(sizeof(struct node));
	h->key=hm[0].key;
	h->prob=hm[0].prob;
	h->next=NULL;
	t=h;
	int i=1;
	int len=length;
	while(len-1)
	{
		h->next=(struct node *)malloc(sizeof(struct node ));
		h=h->next;
		h->key=hm[i].key;
		h->prob=hm[i].prob;
//		printf("\n%d--%f",h->key,h->prob);
		len--;
		h->next=NULL;
		i++;
	}
	t=lsort(t);
	printf("\n\n");
	print(t);
	while(t->next!=NULL)
	{
		printf("\n\n");
		t=reduce(t);
		t=lsort(t);
		print(t);
	}
//	printf("Huffman Codes are-->\n");
	code(b,t);  
}

struct node *lsort(struct node *h)   //Func to sort given linked list Polynomial//
{
	struct node *x,*t,*z;
 	x=h;
 	t=h->next;
 	int cnt=0;
	while(x!= NULL)
	{
		cnt++;
		x=x->next;
	}
	if(cnt==1)
		return h;
 	while(cnt>=0)
 	{
		x=h;
		t=h->next;
		cnt--;
		if(x->prob > t->prob)
		 {
			x->next=t->next;
			t->next=x;
			h=t;
		 }
		z=h;
		x=h->next;
		t=x->next;

		while(t!=NULL)
		 {
			if(x->prob > t->prob)
			 {
				z->next=t;
				x->next=t->next;
				t->next=x;
			 }
			z=z->next;
			x=z->next;
			t=x->next;
		}
  	}
	return h;
}
struct node *reduce(struct node *t)    //Func to add two minimum probabilty nodes//
{
	struct node *temp,*p;
	p=t;
	temp=(struct node *)malloc(sizeof(struct node));
	temp->prob=(p->prob) + (p->next->prob);
	temp->key='x';
	p->parent=temp;
	p->next->parent=temp;
	temp->right=p->next;
	temp->left=p;
	temp->next=p->next->next;
	p->next->next=NULL;
	p->next=NULL;
	p=temp;
	return p;  
}
int paw(int a,int i)   //Power func//
{	int j=1;
	if(i==0)
		return 1;
	while(i)
	{
		j*=a;
		i--;
	}
	return j;
}
void code(char a[3000], struct node *p)   //Evaluate and print the code of leaf-nodes//
{
	int i=0;
	char b[3000];
	while(a[i]!='\0')
		b[i]=a[i++];
	b[i]='\0';
	if(p->key!=120)
		printf("%d --> %s \n",p->key,b);
	else
	{
		i=strlen(b);
		b[i+1]='\0';
		b[i]='0';
		code(b,p->left);
		b[i]='1';
		code(b,p->right);
	}
}
	
	void dct(float *a)	
{
complex fft[3000];
	
	int i;
	for(i=0;i<n/4;i++)
	{
//		printf("\t%f",a[i]);
	}
//	scanf("%d",&n);
	k=n/2;	
	for(i=0;i<k;i++)
	{
	fft[i].r=Sum(1,0,i,a).r;
	fft[i].c=Sum(1,0,i,a).c;
	dct1[i]=(cos(pi*i*2/(n))*(fft[i].r)+sin(pi*i*2/(n))*(fft[i].c));
//	printf("\n\nX_fft(%d)=%f+i%f\n",i,fft[i].r,fft[i].c);
	}
}
complex Sum(int t, int d, int k, float *a)
{
	if(t!=(n/4))
	{

		int s;
		
		complex x;
		s=d+t;
		t=2*t;

		x.r=Sum(t,d,k,a).r+(Sum(t,s,k,a).r)*cos(pi*k*t*2/n)+(Sum(t,s,k,a).c)*sin(pi*k*t*2/n);
		x.c=Sum(t,d,k,a).c+cos(pi*k*t*2/n)*(Sum(t,s,k,a).c)-sin(pi*k*t*2/n)*(Sum(t,s,k,a).r);
		return x;
	}
	complex z;
	z.c=(-1)*sin(pi*k)*a[t+d];
	z.r=a[t+d]*cos(pi*k)+a[d];
	return z;
}
void dst(float *a)	
{
    complex fft[3000];
		
	int i;
	float temp;	
	for(i=0;i<k/2;i++)
	{
		if(i%2==0)
		a[i]=a[i];
		else a[i]=-a[i];
	}
//	for(i=0;i<k;i++)
//		printf("\n%f",a[i]);
	
	for(i=0;i<k;i++)
	{
	fft[i].r=Sum(1,0,i,a).r;
	fft[i].c=Sum(1,0,i,a).c;
	dst1[i]=(cos(pi*i*2/(n))*(fft[i].r)+sin(pi*i*2/(n))*(fft[i].c));
//	printf("\nfft[%d]=%f+i%f",i,fft[i].r,fft[i].c);
	}
//	printf("\n\nreversed-->");
	for(i=0;i<n/4;i++)
	{
 		printf("\n%f ",dst1[i] );	
	}
	
	
}
void mdct(void)
{
	int i,p1,p2;
	float c[3000];
	p1=1;
	p2=n/2-1;
	for(i=0;i<n/2;i++)
	{
		if(i<n/4)
		c[i]=dct1[i];
		else
		c[i]=dst1[i-n/4];
	}
	
	z[0]=c[0]/sqrt(2.0);
	//printf("\nZZ%f\n",z[0]);
	for(i=1;i<n/2;i++)
	{
		if(i<n/4)
		{ if(p2%2==1)
			z[2*i]=(-c[p1++]+c[p2--])/sqrt(2.0);
		  else z[2*i]=(c[p1++]-c[p2--])/sqrt(2.0);
		}
		else if(i>n/4){
			if(p1%2==1)
			z[2*i]=(c[p1++]+c[p2--])/sqrt(2.0);
		  else z[2*i]=(-c[p1++]-c[p2--])/sqrt(2.0);
		}
		else if(i==n/4)
		{z[2*i]=c[i]/sqrt(2.0);
		p1++;
		p2--;}
	//	printf("\nZZ%f\n",z[2*i]);
	}
	for(i=0;i<n/4;i++)
	{
		mdct_final[2*i]=z[2*i];
		mdct_final[2*i+1]=-z[n-2*i-2];	
	}
	for(i=0;i<n/2;i++)
	mdct_final[n-i-1]=-mdct_final[i];
	
	mdct_final[n/2]=-mdct_final[n/2];
	mdct_final[(n/2)-1]=-mdct_final[(n/2)-1];
//	for(i=0;i<n;i++)
	//printf("\nmdctfinal(%d)%f",i,mdct_final[i]);	
}
void quant()
{
	int i=0;
	printf("\n\n");
	for(i=0;i<n;i++)
	{
		if(mdct_final[i]<0)
			quan[i]=floor((-1)*pow(abs(mdct_final[i]),.75));
		else
			quan[i]=floor(pow(mdct_final[i],.75));
		printf("\nquantized(%d)%d",i,quan[i]);
	}
	
}
void IMDCT()
{
	int i=0;
	int k=0;
	for( i=0;i<2*n;i++)
   {
   			 IMDCT1[i]=0;
			for (k=0;k<n;k++)
    {
		IMDCT1[i]=IMDCT1[i]+quan[k]*cos((pi/n)*(i+0.5+0.5*n)*(0.5+k));
    }
    printf("\n\nIMDCT--->%f",IMDCT1[i]);
}

}

	
	
