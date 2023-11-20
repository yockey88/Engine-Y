/**
 * \file script_map.hpp
 * \author Y
 * \brief Contains the ScriptMap class, which is used to cache Mono classes,
 * methods, fields, etc. \details architected (and mostly copied from) the Hazel
 * Engine, see TheCherno youtube channel, first attempt at a scripting engine,
 * mostly a warmup for my own scripting engine
 */

#ifndef ENGINEY_SCRIPT_MAP_HPP
#define ENGINEY_SCRIPT_MAP_HPP

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "mono/metadata/assembly.h"

#include "core/UUID.hpp"
#include "core/hash.hpp"
#include "scripting/script_base.hpp"

#define ENGINEY_INTERNAL_CLASS(name) ScriptMap::GetClassByName("YE." #name)
#define ENGINEY_CACHED_CLASS(name) ScriptMap::GetClassByName(name)
#define ENGINEY_CACHED_CLASS_RAW(name) ScriptMap::GetClassByNameRaw(name)->klass
#define ENGINEY_CACHED_METHOD(klass, name, param_count)                        \
  ScriptMap::GetMethod(ScriptMap::GetClassByName(klass), name, param_count)
#define ENGINEY_CACHED_FIELD(klass, name)                                      \
  ScriptMap::GetFieldByName(ScriptMap::GetClassByName(klass), name)

namespace EngineY {

class ScriptMap {

  static void StoreCoreClasses();
  static void StoreClass(std::string_view name, MonoClass *klass);
  static void StoreClassMethods(ScriptObject &klass);
  static void StoreClassFields(ScriptObject &klass);
  static void StoreClassProperties(ScriptObject &klass);

public:
  ScriptMap() {}
  ~ScriptMap() {}

  static ScriptObject *GetClassByName(const std::string &name);
  static ScriptObject *GetClassByID(UUID32 id);
  static ScriptObject *GetClass(MonoClass *klass);
  static ScriptObject *GetObjectFromClass(MonoObject *obj);

  static ScriptMethod *GetMethodByName(ScriptObject *obj,
                                       const std::string &name,
                                       bool virtualm = false);
  static ScriptMethod *GetMethod(ScriptObject *obj, const std::string &name,
                                 uint32_t num_params, bool virtualm = false);

  static ScriptField *GetFieldByName(const ScriptObject *obj,
                                     const std::string &name);
  static ScriptField *GetFieldByID(const ScriptObject *obj, UUID32 id);
  static MonoClass *GetFieldClass(FieldType field);
  static MonoObject *GetFieldValueObj(MonoObject *klass, ScriptField *field);

  static bool CheckFieldVisibility(const ScriptObject *object,
                                   const ScriptField *field);

  static void Generate();
  static void LoadProjectTypes();

  static void UnloadProjectTypes();
  static void Destroy();

#ifdef ENGINEY_DEBUG
  static void PrintAll();
#endif
};

} // namespace EngineY

#endif // !YE_SCRIPT_MAP_HPP
