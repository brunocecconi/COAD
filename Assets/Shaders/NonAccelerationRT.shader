#version 450

#if VERTEX_SHADER

layout(location = 0) in vec2 Position;
layout(location = 0) out vec2 UV;

void main()
{
	gl_Position = vec4(Position, 0.0, 1.0);
	UV = ((Position + 1.0) * 0.5);
	UV.y = 1.0 - UV.y;
}

#endif

#if FRAGMENT_SHADER

layout(location = 0) out vec4 OutColor;
layout(location = 0) in vec2 UV;

layout(binding = 0) uniform ScreenPropertiesS
{
	vec2 Resolution;
} ScreenProperties;

#define ASPECT_RATIO	(ScreenProperties.Resolution.x / ScreenProperties.Resolution.y)

float ViewportHeight = 2.0;
float ViewportWidth = ViewportHeight * ASPECT_RATIO;
float FocalLength = 1.0;

vec3 RayOrigin;
vec3 RayHorizontal;
vec3 RayVertical;
vec3 RayUpperLeftCorner;

void SetupCamera()
{
	RayOrigin			 = vec3(0, 0, 0);
	RayHorizontal		 = vec3(ViewportWidth, 0, 0);
	RayVertical			 = vec3(0, ViewportHeight, 0);
	RayUpperLeftCorner = RayOrigin + vec3(-ViewportWidth * 0.5, -ViewportHeight * 0.5, -FocalLength);
}

float HitSphere(vec3 Center, float Radius, vec3 RayDirection)
{
	vec3 oc			  = RayOrigin - Center;
	float a			  = dot(RayDirection, RayDirection);
	float b			  = 2.0f * dot(oc, RayDirection);
	float c			  = dot(oc, oc) - Radius * Radius;
	float discriminant = b * b - 4.0 * a * c;
	if (discriminant < 0.f)
	{
		return -1.f;
	}
	return (-b - sqrt(discriminant)) / (2.f * a);
}

vec3 RayAt(vec3 Direction, float Length)
{
	return RayOrigin + Direction*Length;
}

vec3 RayColor(vec3 Direction)
{
	float RayLength = HitSphere(vec3(0.f, 0.f, -1.f), 0.5, Direction);
	if (RayLength > 0.f)
	{
		vec3 Normal = normalize(RayAt(Direction, RayLength) - vec3(0.f, 0.f, -1.f));
		return 0.5f * (Normal + vec3(1.f, 1.f, 1.f));
	}
	
	vec3 UnitDir = normalize(Direction);
	RayLength					= 0.5f * (UnitDir.y + 1.0f);
	return (1.0f - RayLength) * vec3(0.0f, 0.0f, 0.0f) + RayLength * vec3(0.5f, 0.7f, 1.0f);
}

void main()
{
	SetupCamera();
	vec3 RayDirection = RayUpperLeftCorner + UV.x * RayHorizontal + UV.y * RayVertical - RayOrigin;
	OutColor = vec4(RayColor(RayDirection), 1.0);
}

#endif
