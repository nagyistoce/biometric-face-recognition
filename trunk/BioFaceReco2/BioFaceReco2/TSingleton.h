#pragma once

template < class T >
class TSingleton
{
public:
	static T& Instance()
	{
		static T singleton;
		return singleton;
	}
};