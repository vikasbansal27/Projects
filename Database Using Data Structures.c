/* This database is implemeted using structures. A node contains an employee number, his/her age, salary, name and address.
It provides functions such as sorting on basis of employee number, list of 5 figures salary employee, avg salary of an 
employee and details of employee. */


#include<stdio.h>
struct node{
			int en;
			float age;
			float sal;
			char name[15];
			char addr[20];
           };
		   
void sorting_en(struct node *,int);
void figure5_salary(struct node *,int);
void avg_salary(struct node *,int);
void details(struct node *,int);
void print(struct node *,int);
void menu(struct node *,int);

int main()
{
	int n,i,a;
	struct node x[50];
	printf("Give number of employes\n");
	scanf("%d",&n);
	for(i=0;i<n;i++)
	{
		printf("\nGive %d employee number\n",i+1);
		scanf("%d",&(x[i].en));
		printf("\nGive %d employee name\n",i+1);
		scanf("%s",x[i].name);
	        printf("\nGive %d employee age\n",i+1);
		scanf("%f",&(x[i].age));
		printf("\nGive %d employee address\n",i+1);
    	        scanf("%s",x[i].addr);
		printf("\nGive %d employee salary\n",i+1);
		scanf("%f",&(x[i].sal));
	}
	menu(x,n);

return 0;
}
void sorting_en(struct node *x,int n)
{
	struct node temp;
	int i,j;
	for(i=0;i<n;i++)
	{	
		for(j=0;j<n;j++)
		{
			if(x[j].en>x[j+1].en)
			{ 
				temp=x[j];
				x[j]=x[j+1];
				x[j+1]=temp;			
			}
		}
	}
		for(i=0;i<n;i++)
	{
		printf("Employee number%d\n",x[i].en);
		printf("Age %f\n",x[i].age);
		printf("Salary %f\n",x[i].sal);
		printf("Name %s\n",x[i].name);
		printf("Address %s\n\n",x[i].addr);
	}
	
	menu(x,n);			
}
void figure5_salary(struct node *x,int n)
{

	int i;

	for(i=0;i<n;i++)
		if(((x[i].sal)<=99999)&&(x[i].sal>=10000))
		{
			printf("Employee number %d\n",x[i].en);
			printf("Age %f\n",x[i].age);
			printf("Salary %f\n",x[i].sal);
			printf("Name %s\n",x[i].name);
			printf("Address %s\n\n",x[i].addr);	
		}

	menu(x,n);
}
void avg_salary(struct node *x,int n)
{
   int i;
   float s=0;
   for(i=0;i<n;i++)
		s=s+x[i].sal;
	printf("\n%f",(s/n));
	menu(x,n);
}
void details(struct node *x,int n)
{
	int c,j;
	
	printf("\nGive employe number\n");
	scanf("%d",&c);
	
	j=binary(x, 0, n-1 ,c);
	
	if(j==-1);
	else{
			printf("Employee number %d\n",x[j].en);
			printf("Age %f\n",x[j].age);
			printf("Salary %f\n",x[j].sal);
			printf("Name %s\n",x[j].name);
			printf("Address %s\n\n",x[j].addr);	
		}
	menu(x,n);
}
int binary(struct node *x, int p, int q ,int num)
{
	int i=p+q/2;
	
	if(p>q)
	{
		printf("\nNo such employee\n");
		return -1;
	}
	if(x[i].en==num)
		return i;
	else if(x[i].en>num)
		 return binary(x,p,i-1,num);
		 else if(x[i].en<num)
		      return binary(x,i+1,q,num);
	
}
void menu(struct node *x,int n)
{
	int a;
	
	printf("\nPress 1 for sorting on basis of en \n");
    printf("\nPress 2 for list of 5 figures salary employe \n");
	printf("\nPress 3 for avg salary of employe \n");
	printf("\nPress 4 for details of emplyee \n");
	printf("\nPress 5 exit \n");
	
	scanf("%d",&a);
	if(a==1)
		sorting_en(x,n);
	else if(a==2)
			figure5_salary(x,n);
		 else if(a==3)
				avg_salary(x,n);
			   else if(a==4)
						details(x,n);
					else if(a==5)
							exit();
}
	
	
	
	
