#pragma once

#include "Scene.h"
#include "ConcaveHexahedron.h"
#include "PubeScreenTransformer.h"
#include "Mat3.h"

class ConHexScene : public Scene
{
public:
	ConHexScene()
		:
		Scene( "Concave Hexahedron Nasty Draw Order" )
	{}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) override
	{
		if( kbd.KeyIsPressed( Keyboard::Q ) )
		{
			theta_x = wrap_angle( theta_x + dTheta * dt );
		}
		if( kbd.KeyIsPressed( Keyboard::W ) )
		{
			theta_y = wrap_angle( theta_y + dTheta * dt );
		}
		if( kbd.KeyIsPressed( Keyboard::E ) )
		{
			theta_z = wrap_angle( theta_z + dTheta * dt );
		}
		if( kbd.KeyIsPressed( Keyboard::A ) )
		{
			theta_x = wrap_angle( theta_x - dTheta * dt );
		}
		if( kbd.KeyIsPressed( Keyboard::S ) )
		{
			theta_y = wrap_angle( theta_y - dTheta * dt );
		}
		if( kbd.KeyIsPressed( Keyboard::D ) )
		{
			theta_z = wrap_angle( theta_z - dTheta * dt );
		}
		if( kbd.KeyIsPressed( Keyboard::R ) )
		{
			offset_z += 2.0f * dt;
		}
		if( kbd.KeyIsPressed( Keyboard::F ) )
		{
			offset_z -= 2.0f * dt;
		}
	}
	virtual void Draw( Graphics& gfx ) const override
	{
		// generate indexed triangle list
		auto triangles = hex.GetTriangles();
		// generate rotation matrix from euler angles
		const Mat3 rot =
			Mat3::RotationX( theta_x ) *
			Mat3::RotationY( theta_y ) *
			Mat3::RotationZ( theta_z );
		// transform from model space -> world (/view) space
		for( auto& v : triangles.vertices )
		{
			v *= rot;
			v += { 0.0f,0.0f,offset_z };
		}
		// backface culling test (must be done in world (/view) space)
		for( size_t i = 0,
			 end = triangles.indices.size() / 3;
			 i < end; i++ )
		{
			const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]];
			const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]];
			const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]];
			triangles.cullFlags[i] = (v1 - v0) % (v2 - v0) * v0 > 0.0f;
		}
		// transform to screen space (includes perspective transform)
		for( auto& v : triangles.vertices )
		{
			pst.Transform( v );
		}
		// draw the mf triangles!
		for( size_t i = 0,
			 end = triangles.indices.size() / 3;
			 i < end; i++ )
		{
			// skip triangles previously determined to be back-facing
			if( !triangles.cullFlags[i] )
			{
				gfx.DrawTriangle(
					triangles.vertices[triangles.indices[i * 3]],
					triangles.vertices[triangles.indices[i * 3 + 1]],
					triangles.vertices[triangles.indices[i * 3 + 2]],
					colors[i] );
			}
		}
	}
private:
	PubeScreenTransformer pst;
	ConcaveHexahedron hex = ConcaveHexahedron( 1.0f );
	static constexpr Color colors[12] = {
		Colors::White,
		Colors::Blue,
		Colors::Cyan,
		Colors::Yellow,
		Colors::Green,
		Colors::Magenta,
	};
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};