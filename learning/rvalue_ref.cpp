// g++ -O0 -fno-elide-constructors -std=c++11 rvalue_ref.cpp -o rvalue_ref
// no-elide-constructors force GCC into non-optimizing temporary object returns

#include <iostream>
#include <string>

using std::cout;

struct Object {
	Object(): id(gid++) { cout << "Object " << id << " created\n"; }
	Object(const std::string &s): id(gid++), name(s) {
		cout << "Object " << id << " \"" << name << "\" created\n";
	}
	Object(const Object &o): id(gid++), name(o.name) {
		if (name.empty())
			cout << "Object " << id << " copy-const from " << o.id << "\n";
		else
			cout << "Object " << id << " copy-const from " << o.id << " \"" << name << "\"\n";
	}
	Object &operator=(const Object &o) {
		cout << "-Copying " << o.id << " to " << id << "\n";
	}
	~Object() {
		if (name.empty())
			cout << "Object " << id << " destroyed\n";
		else
			cout << "Object " << id << " \"" << name << "\" destroyed\n";
	}
	std::string get() const { return name; }
	void set(std::string n) { name = n; }
private:
	int id;
	std::string name;
	static int gid;
};

int Object::gid = 0;

Object fun(const std::string &n="") {
	return Object(n); // Build an object
}

int main(int, char**) {
	{
		Object o0;
		Object o1("o1");
		Object o2(o0);
		Object o3(o1);
	}
	cout << "\n";
	{
		fun("t0"); // Create a temporary object using returned-by-value
	}
	cout << "\n";
	{
		Object r0 = fun("t1");
	}
	cout << "\n";
	{
		Object r1("r1");
		r1 = fun("t2");
	}
	cout << "\n";
	{
		const Object &r = fun("t3");
		std::cout << "-Working with " << r.get() << "\n";
	}
	cout << "\n";
	{
		Object &&r = fun("t4");
		std::cout << "-Working with " << r.get() << "\n";
		r.set("foo");
		std::cout << "-Working with " << r.get() << "\n";
	}
	
	return 0;
}
