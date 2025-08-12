#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/mono-config.h"
#include <string>
#include <iostream>


void main() 
{
	MonoDomain* domain = mono_jit_init("MonoTestDomain");
	const char* assemblyPath = "C:\\Users\\61995\\Desktop\\MonoTest\\x64\\Debug\\GameScripts.dll"; // Update with your assembly path
	MonoAssembly* assembly = mono_domain_assembly_open(domain, assemblyPath);
	if (!assembly) {
		printf("Failed to load assembly: %s\n", assemblyPath);
		return;
	}

	MonoImage* image = mono_assembly_get_image(assembly);
	MonoClass* klass = mono_class_from_name(image, "GameScripts", "HelloWorld");
	MonoMethod* method = mono_class_get_method_from_name(klass, "SayHello", 0);

	mono_runtime_invoke(method, nullptr, nullptr, nullptr);

	std::string name = "World"; // Example name to pass to the method
	MonoMethod* method1 = mono_class_get_method_from_name(klass, "GetMessage", 1);

	MonoString* mono_name = mono_string_new(domain, name.c_str());
	void* args[1] = { mono_name };

	MonoObject* result = mono_runtime_invoke(method1, nullptr, args, nullptr);
	MonoString* result_string = (MonoString*)result;

	char* utf8_result = mono_string_to_utf8(result_string);
	std::string cpp_result(utf8_result);

	std::cout << cpp_result << std::endl;
	mono_free(utf8_result);
	return;
}