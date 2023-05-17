#pragma once
#ifdef GVV
#pragma GCC diagnostic ignored "-Wformat-signedness"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wnull-dereference"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wformat-overflow"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#pragma GCC diagnostic ignored "-Wsuggest-final-types"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
#pragma GCC diagnostic ignored "-Wstrict-null-sentinel"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wduplicated-cond"
#endif
#if defined(VALIJSON) || defined(JSON)
#include <json/json.h>
#endif
#ifdef VALIJSON
#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/utils/jsoncpp_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>
#endif

#ifdef GCC
#pragma GCC diagnostic warning "-Wformat-signedness"
#pragma GCC diagnostic warning "-Wuseless-cast"
#pragma GCC diagnostic warning "-Wold-style-cast"
#pragma GCC diagnostic warning "-Wcast-qual"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wcast-align"
#pragma GCC diagnostic warning "-Wnull-dereference"
#pragma GCC diagnostic warning "-Wredundant-decls"
#pragma GCC diagnostic warning "-Wextra-semi"
#pragma GCC diagnostic warning "-Wformat-security"
#pragma GCC diagnostic warning "-Wformat-overflow"
#pragma GCC diagnostic warning "-Wsuggest-override"
#pragma GCC diagnostic warning "-Wsuggest-final-methods"
#pragma GCC diagnostic warning "-Wsuggest-final-types"
#pragma GCC diagnostic warning "-Wnon-virtual-dtor"
#pragma GCC diagnostic warning "-Wctor-dtor-privacy"
#pragma GCC diagnostic warning "-Wstrict-null-sentinel"
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wduplicated-cond"
#endif
