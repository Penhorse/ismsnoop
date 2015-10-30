#include "library.h"

#include <dlfcn.h>

std::string Library::get_filename(const std::string & name)
{
	return std::string("lib") + name + ".so";
}

void Library::close(void * library)
{
	dlclose(library);
}

void * Library::load(const std::string & path)
{
	return dlopen(path.c_str(), RTLD_LAZY);
}

void * Library::get_function(void * library, const std::string & name)
{
	return dlsym(library, name.c_str());
}
