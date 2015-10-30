#pragma once

#include <deque>
#include <functional>
#include <string>

class Library
{

public:

	Library(const std::string & name);
	~Library();

	bool load(const std::deque<std::string> & search_paths = {});

	template <class Signature>
	std::function<Signature> get_function(const std::string & name) const;

	bool loaded() const;

	static std::string get_filename(const std::string & name);

private:

	static void close(void * library);
	static void * load(const std::string & path);
	static void * get_function(void * library, const std::string & name);

	std::string name_;
	void *      library_;

};

template <class Signature>
std::function<Signature> Library::get_function(const std::string & name) const
{
	return library_ ? reinterpret_cast<Signature*>(get_function(library_, name)) : nullptr;
}

