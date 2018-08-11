#ifndef __PROPERTY_COMM_H__
#define __PROPERTY_COMM_H__

#define PROP_DEFBEGIN()\
	enum\
	{

#define PROP_DEFIMP(id, type, value, desc)\
	id = value,

#define PROP_SIMPLEDEFIMP(id, type, desc)\
	id,


#define PROP_DEFEND()	};

#endif

