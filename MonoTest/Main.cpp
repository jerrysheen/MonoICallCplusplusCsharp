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
MonoDomain* domain;
MonoAssembly* assembly;
MonoImage* image;


struct ManagedObjectLayout
{
public:
	void* nativePtr;      // 对应 IntPtr nativePtr
	void* testObject;      // 对应 int testValue  
};

extern "C"
{
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

	void ManagedCallSetFloatArray(float* data, int length, float scale)
	{
		for (int i = 0; i < length; ++i)
		{
			data[i] *= scale;
			std::cout << data[i] << std::endl;
		}

	}

	void SetManagedObjectValue(MonoObject* obj) 
	{
		char* ptr = (char*)obj;
		ptr += (size_t)(sizeof(void*) * 2);
		ManagedObjectLayout* layout = reinterpret_cast<ManagedObjectLayout*>(ptr);
		layout->testObject = (void*)12121212;
	}

	MonoObject* NativeDirectCreateNewAndReturn() 
	{
		auto newComp = new Component(111, mono_string_new(domain, "1111"));
		componentPool.insert(newComp);
		// 这个地方应该就是创建一个对应C# 名字的Class， 然后对应的数据填进去。
		MonoClass* mainClass = mono_class_from_name(image, "GameScripts", "Component");



		// 创建对象 + 初始化
		MonoObject* obj = mono_object_new(domain, mainClass);
		mono_runtime_object_init(obj);

		// 塞对应的IntPtr值给 C#类， 销毁的时候才能找到对应的C++，
		MonoClassField* idField = mono_class_get_field_from_name(mainClass, "_nativeHandle");
		if (idField) {
			void* value = newComp;
			mono_field_set_value(obj, idField, &value);
		}
		return obj;
	}

}


void main() 
{
	domain = mono_jit_init("MonoTestDomain");
	const char* assemblyPath = "D:\\GitHubST\\MonoICallCplusplusCsharp\\x64\\Debug\\GameScripts.dll"; // Update with your assembly path
	assembly = mono_domain_assembly_open(domain, assemblyPath);
	if (!assembly) {
		printf("Failed to load assembly: %s\n", assemblyPath);
		return;
	}

	image = mono_assembly_get_image(assembly);

	mono_add_internal_call("GameScripts.Component::NativeCallGetId", reinterpret_cast<void*>(ManagedCallComponentGetId));
	mono_add_internal_call("GameScripts.Component::NativeCallGetTagInternal", reinterpret_cast<void*>(ManagedCallComponentGetTag));
	mono_add_internal_call("GameScripts.Component::NativeCallComponentNew", reinterpret_cast<void*>(ManagedCallComponentNew));
	mono_add_internal_call("GameScripts.Component::NativeCallComponentDelete", reinterpret_cast<void*>(ManagedCallComponentDelete));
	mono_add_internal_call("GameScripts.Component::NativeCallSetFloatArray", reinterpret_cast<void*>(ManagedCallSetFloatArray));
	mono_add_internal_call("GameScripts.Component::NativeDirectCreateNewAndReturn", reinterpret_cast<void*>(NativeDirectCreateNewAndReturn));
	mono_add_internal_call("GameScripts.Component::SetManagedObjectValue", reinterpret_cast<void*>(SetManagedObjectValue));

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