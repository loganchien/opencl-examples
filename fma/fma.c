#include <CL/cl.h>

#include <stdio.h>
#include <stdlib.h>

int read_file(unsigned char **output, size_t *size, const char *name) {
  FILE* fp = fopen(name, "rb");
  if (!fp) {
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  *output = (unsigned char *)malloc(*size);
  if (!*output) {
    fclose(fp);
    return -1;
  }

  fread(*output, *size, 1, fp);
  fclose(fp);
  return 0;
}

void run_vec_fma(size_t num_elems, size_t buf_size, cl_int* data) {
  cl_int err;

  // Query platforms and devices
  cl_platform_id platform;
  err = clGetPlatformIDs(1, &platform, NULL);

  cl_device_id device;
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);

  const cl_context_properties prop[] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
    0
  };

  // Create context
  cl_context ctx = clCreateContext(prop, 1, &device, NULL, NULL, &err);

  // Create program
  unsigned char* program_file = NULL;
  size_t program_size = 0;

  cl_program prog[2];

  // Comple source - elem.cl
  read_file(&program_file, &program_size, "elem.cl");
  prog[0] = clCreateProgramWithSource(ctx, 1, (const char **)&program_file,
                                      &program_size, &err);
  err = clCompileProgram(prog[0], 1, &device, NULL, 0, NULL, NULL, NULL, NULL);
  free(program_file);

  // Compile source - fma.cl
  read_file(&program_file, &program_size, "fma.cl");
  prog[1] = clCreateProgramWithSource(ctx, 1, (const char **)&program_file,
                                      &program_size, &err);
  err = clCompileProgram(prog[1], 1, &device, NULL, 0, NULL, NULL, NULL, NULL);
  free(program_file);

  // Link programs
  cl_program program =
      clLinkProgram(ctx, 1, &device, NULL, 2, prog, NULL, NULL, &err);

  // Allocate memory buffers (on the device)
  cl_mem a = clCreateBuffer(ctx, CL_MEM_READ_ONLY, buf_size, NULL, &err);
  cl_mem b = clCreateBuffer(ctx, CL_MEM_READ_ONLY, buf_size, NULL, &err);
  cl_mem c = clCreateBuffer(ctx, CL_MEM_READ_ONLY, buf_size, NULL, &err);
  cl_mem d = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, buf_size, NULL, &err);

  // Create command queue
  cl_command_queue queue = clCreateCommandQueue(ctx, device, 0, NULL);

  // Enqueue the write buffer commands
  cl_event wb_events[2];

  err = clEnqueueWriteBuffer(queue, a, CL_FALSE, 0, buf_size, data, 0,
                             NULL, &wb_events[0]);
  err = clEnqueueWriteBuffer(queue, b, CL_FALSE, 0, buf_size, data, 0,
                             NULL, &wb_events[1]);
  err = clEnqueueWriteBuffer(queue, c, CL_FALSE, 0, buf_size, data, 0,
                             NULL, &wb_events[1]);

  // Enqueue the kernel execution command
  cl_kernel kernel = clCreateKernel(program, "vec_fma", &err);
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d);
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &a);
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &b);
  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &c);

  const size_t global_offset = 0;
  cl_event kernel_event;
  err = clEnqueueNDRangeKernel(queue, kernel, 1, &global_offset,
                               &num_elems, NULL, 2, wb_events,
                               &kernel_event);

  // Enqueue the read buffer command
  err = clEnqueueReadBuffer(queue, d, CL_TRUE, 0, buf_size, data, 1,
                            &kernel_event, NULL);

  // Wait until every commands are finished
  err = clFinish(queue);

  // Release the resources
  clReleaseMemObject(a);
  clReleaseMemObject(b);
  clReleaseMemObject(c);
  clReleaseMemObject(d);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(ctx);
  clReleaseDevice(device);
}

int main() {
  cl_int i;

  // Prepare the input data
  const size_t num_elems = 10000000;
  const size_t buf_size = sizeof(cl_int) * num_elems;
  cl_int *data = (cl_int *)malloc(buf_size);
  for (i = 0; i < num_elems; ++i) {
    data[i] = i;
  }

  // Run the OpenCL program
  run_vec_fma(num_elems, buf_size, data);

  // Check the answer
  for (i = 0; i < num_elems; ++i) {
    if (data[i] != i * i + i) {
      fprintf(stderr, "Failed: %u\n", (unsigned)i);
    }
  }

  return 0;
}
