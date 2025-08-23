#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/mono-config.h"
#include <string>
#include <iostream>
#include <set>

class Component 
{
public:
	int id;
	MonoString* tag;

	Component(int id, MonoString* tag) : id(id), tag(tag)
	{
		std::cout << "Component : " << id << "constructed in native code!" << std::endl;
	}

	~Component() 
	{
		std::cout << "Component : " << id << "deconstructed in native code!" << std::endl;
	}
};

std::set<Component*> componentPool;

int ManagedCallComponentGetId(const Component* component) {
	return component->id;
}

MonoString* ManagedCallComponentGetTag(const Component* component) {
	return component->tag;
}

Component* ManagedCallComponentNew(int test, int id, MonoString* tag) {
	auto newComp = new Component(id, tag);
	componentPool.insert(newComp);
	return newComp;
}

void ManagedCallComponentDelete(Component* component) {
	componentPool.erase(component);
	delete component;
}


void main() 
{
	MonoDomain* domain = mono_jit_init("MonoTestDomain");
	const char* assemblyPath = "D:\\GitHubST\\MonoICallCplusplusCsharp\\x64\\Debug\\GameScripts.dll"; // Update with your assembly path
	MonoAssembly* assembly = mono_domain_assembly_open(domain, assemblyPath);
	if (!assembly) {
		printf("Failed to load assembly: %s\n", assemblyPath);
		return;
	}

	MonoImage* image = mono_assembly_get_image(assembly);
	//MonoClass* klass = mono_class_from_name(image, "GameScripts", "HelloWorld");
	//MonoMethod* method = mono_class_get_method_from_name(klass, "SayHello", 0);
	//
	//mono_runtime_invoke(method, nullptr, nullptr, nullptr);
	//
	//std::string name = "World"; // Example name to pass to the method
	//MonoMethod* method1 = mono_class_get_method_from_name(klass, "GetMessage", 1);

	//MonoString* mono_name = mono_string_new(domain, name.c_str());
	//void* args[1] = { mono_name };

	//MonoObject* result = mono_runtime_invoke(method1, nullptr, args, nullptr);
	//MonoString* result_string = (MonoString*)result;

	//char* utf8_result = mono_string_to_utf8(result_string);
	//std::string cpp_result(utf8_result);

	//std::cout << cpp_result << std::endl;
	//mono_free(utf8_result);

	mono_add_internal_call("GameScripts.Component::NativeCallGetId", reinterpret_cast<void*>(ManagedCallComponentGetId));
	mono_add_internal_call("GameScripts.Component::NativeCallGetTagInternal", reinterpret_cast<void*>(ManagedCallComponentGetTag));
	mono_add_internal_call("GameScripts.Component::NativeCallComponentNew", reinterpret_cast<void*>(ManagedCallComponentNew));
	mono_add_internal_call("GameScripts.Component::NativeCallComponentDelete", reinterpret_cast<void*>(ManagedCallComponentDelete));

	MonoClass* componentClass = mono_class_from_name(image, "GameScripts", "Component");
	MonoClassField* nativeHandleField = mono_class_get_field_from_name(componentClass, "_nativeHandle");

	MonoClass* mainClass = mono_class_from_name(image, "GameScripts", "ManagedClass");
	MonoMethodDesc* entryPointMethodDesc = mono_method_desc_new("GameScripts.ManagedClass:Main()", true);
	MonoMethod* entryPointMethod = mono_method_desc_search_in_class(entryPointMethodDesc, mainClass);

	mono_method_desc_free(entryPointMethodDesc);

	mono_runtime_invoke(entryPointMethod, nullptr, nullptr, nullptr);

	mono_jit_cleanup(domain);

	if (componentPool.size() != 0)
	{
		for (auto comp : componentPool)
		{
			std::cout << "WARNING: Component #" << comp->id << " constructed in native code (May cause by memory leak)" << std::endl;
			delete comp;
		}

		componentPool.clear();
	}
	return;
}