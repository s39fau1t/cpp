#include <bgfx/bgfx_compute.sh>

//https://stackoverflow.com/questions/16069959/glsl-how-to-ensure-largest-possible-float-value-without-overflow
#define INFINITY 3.402823466e+38


/*




TODO: Make this nicer




*/



//TODO: Why can't I use Sphere directly?
BUFFER_RO(sphereData, float, 0);

IMAGE2D_WR(pixels, rgba8, 1);

NUM_THREADS(1, 1, 1)

struct Sphere {
  vec3 center;
  float radius;
  vec3 color;
};

void putPixel(int x, int y, vec3 color) {
  imageStore(pixels, ivec2(x, y), vec4(color / 255, 1));
}

vec3 pixelsToViewport(int x, int y, ivec2 size) {
  return vec3((float)x / size.x - 0.5, (float)y / size.y - 0.5, 1);
}

Sphere createSphere(int index) {
  int ptr = index * 7 + 1;

  return Sphere(vec3(sphereData[ptr], sphereData[ptr + 1], sphereData[ptr + 2]), sphereData[ptr + 3], vec3(sphereData[ptr + 4], sphereData[ptr + 5], sphereData[ptr + 6]));
}

vec2 intersectRaySphere(vec3 origin, vec3 direction, Sphere sphere) {
  float r = sphere.radius;
  vec3 co = origin - sphere.center;

  float a = dot(direction, direction);
  float b = 2 * dot(co, direction);
  float c = dot(co, co) - r * r;

  float discriminant = b * b - 4 * a * c;
  if(discriminant < 0) {
    return vec2(INFINITY, INFINITY);
  }

  float t1 = (-b + sqrt(discriminant)) / (2 * a);
  float t2 = (-b - sqrt(discriminant)) / (2 * a);

  return vec2(t1, t2);
}

vec3 traceRay(vec3 origin, vec3 direction, float tMin, float tMax) {
  float closestT = INFINITY;
  //-1 radius signifies NULL
  Sphere closestSphere = Sphere(vec3(0, 0, 0), -1, vec3(0, 0, 0));

  for(int i = 0; i < sphereData[0]; i ++) {
    Sphere sphere = createSphere(i);

    vec2 t1t2 = intersectRaySphere(origin, direction, sphere);
    float t1 = t1t2.x;
    float t2 = t1t2.y;

    if(t1 > tMin && t1 < tMax && t1 < closestT) {
      closestT = t1;
      closestSphere = sphere;
    }

    if(t2 > tMin && t2 < tMax && t2 < closestT) {
      closestT = t2;
      closestSphere = sphere;
    }
  }

  if(closestSphere.radius == -1) {
    return vec3(255, 255, 255);
  }

  return closestSphere.color;
}

void main() {
  ivec2 size = imageSize(pixels);

  vec3 origin = vec3(0, 0, 0);

  for(int y = 0; y < size.y; y ++) {
    for(int x = 0; x < size.x; x ++) {
      vec3 direction = pixelsToViewport(x, y, size);

      vec3 color = traceRay(origin, direction, 1,  INFINITY);

      putPixel(x, y, color);
    }
  }
}
