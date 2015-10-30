#include "library.h"

Library::Library(const std::string & name) :
	name_(name),
	library_(nullptr)
{
	// nothing
}

bool Library::load(const std::deque<std::string> & search_paths)
{
	if(library_)
	{
		close(library_);
	}

	if(!search_paths.empty())
	{
		for(const auto & search_path : search_paths)
		{
			const auto full_path = search_path + "/" + name_;

			library_ = load(full_path);

			if(library_) return true;
		}
	}

	return (library_ = load(name_)) != nullptr;
}

bool Library::loaded() const
{
	return library_;
}

Library::~Library()
{
	if(library_)
	{
		close(library_);
	}
}
