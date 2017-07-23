#include<stdio.h>
#include<CL/cl.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define MAX 100000;

int main()
{
	int n,*a,*b,k,i,count=0;
	char *src_str;
	time_t start,end;
	size_t s_size;
	cl_platform_id platform_id=NULL;
	cl_device_id device_id=NULL;
	cl_uint ret_num_devices,ret_num_platform;

	printf("Enter the no. of nodes: ");scanf("%d",&n);
	a=(int *)calloc(n*n,sizeof(int));
	b=(int *)calloc(n*n,sizeof(int));
	
	printf("\nEnter %d X %d distance matrix:\nPUT -1 for INFINITY\n\n",n,n);
	for(i=0;i<n*n;i++)
	{
		scanf("%d",&a[i]);
		if(a[i]==-1)
			a[i]=MAX;
	}

	start=clock();

	FILE *f1=fopen("p1.cl","r");
	if(f1==NULL)
		{
		fprintf(stdout,"File not found: ",n);fflush(stdout);exit(0);
		}
	src_str=(char *)malloc(100000);
	s_size=fread(src_str,sizeof(char),100000,f1);

	fclose(f1);


	
	cl_int status=clGetPlatformIDs(1,&platform_id,&ret_num_platform);
	//printf("%d\n",status);
	status=clGetDeviceIDs(platform_id,CL_DEVICE_TYPE_GPU,1,&device_id,&ret_num_devices);
	//printf("%d\n",status);

	cl_context context=clCreateContext(NULL,1,&device_id,NULL,NULL,&status);
	//printf("%d\n",status);
	cl_command_queue cq=clCreateCommandQueue(context,device_id,CL_QUEUE_PROFILING_ENABLE,&status);
	//printf("%d\n",status);
	cl_mem a_mem_obj = clCreateBuffer(context,CL_MEM_READ_WRITE,n*n*sizeof(int),NULL,&status);
	//printf("%d\n",status);
	
	status= clEnqueueWriteBuffer(cq,a_mem_obj,CL_TRUE,0,n*n*sizeof(int),a,0,NULL,NULL);
	//printf("%d\n",status);

	cl_program program=clCreateProgramWithSource(context,1,(const char **)&src_str,(const size_t *)&s_size,&status);
	//printf("%d\n",status);
		
	status=clBuildProgram(program,1,&device_id,NULL,NULL,NULL);
	
	//printf("%d\n",status);
	cl_kernel kernel1 = clCreateKernel(program,"Floyd1",&status);
	//printf("%d\n",status);
	size_t global_item_size[2]={n,n},local_item_size[2]={1,1};
	status=clSetKernelArg(kernel1,0,sizeof(cl_mem),&a_mem_obj);
	//printf("%d\n",status);
	status=clSetKernelArg(kernel1,1,sizeof(int),&k);
	//printf("%d\n",status);
	for(k=0;k<n;k++)
	{//printf("OK\n");
		count++;
		status=clSetKernelArg(kernel1,1,sizeof(int),&k);
		status=clEnqueueNDRangeKernel(cq,kernel1,2,NULL,global_item_size,local_item_size,0,NULL,NULL);
		status=clFinish(cq);
	}
	status=clEnqueueReadBuffer(cq,a_mem_obj,CL_TRUE,0,n*n*sizeof(int),b,0,NULL,NULL);
	end=clock();
	printf("OUTPUT:\n------\n");
	for(i=0;i<n;i++)
	{
		for(k=0;k<n;k++)
			printf("%d\t",b[i*n+k]);
		printf("\n");
	}
	double t1=(double)(end-start)/CLOCKS_PER_SEC;
	printf("\nTime Taken= %lf\t Count= %d\n",t1,count);
	
	status=clFlush(cq);
	status=clReleaseKernel(kernel1);
	status=clReleaseProgram(program);
	status=clReleaseMemObject(a_mem_obj);
	
	status=clReleaseCommandQueue(cq);
	status=clReleaseContext(context);
	
	free(a);

	getchar();
	getchar();
	getchar();
	return 0;

}