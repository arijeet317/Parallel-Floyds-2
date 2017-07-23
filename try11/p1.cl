__kernel void Floyd1(__global int *a, int k)
{
	int row=get_global_id(0),col=get_global_id(1),n=get_global_size(0),temp;
	//a[row*n+col]=2;
	if(row!=k || col!=k)
	{
		temp=a[row*n+k]+a[k*n+col];
		if(temp < a[row*n+col])
			a[row*n+col]=temp;
	}
}