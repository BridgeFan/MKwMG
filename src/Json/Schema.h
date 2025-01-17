#pragma once
#ifndef MKWMG_SCHEMA_H
#define MKWMG_SCHEMA_H
const std::string schemaStr = R"_JSON_({
	"$schema": "https://json-schema.org/draft/2020-12/schema",

			"type": "object",
			"properties": {
		"camera": {
			"type": "object",
			"properties": {
				"focusPoint": {"$ref": "#/definitions/float3"},
				"distance": {"type": "number"},
				"rotation": {"$ref": "#/definitions/float2"}
			},
			"required": ["focusPoint", "distance", "rotation"],
			"additionalProperties": false
		},
		"points": {
			"type": "array",
					"items": { "$ref": "#/definitions/geometry/point" }
		},
		"geometry": {
			"type": "array",
			"items": {
				"oneOf": [
				{ "$ref": "#/definitions/geometry/torus" },
				{ "$ref": "#/definitions/geometry/bezierC0" },
				{ "$ref": "#/definitions/geometry/bezierC2" },
				{ "$ref": "#/definitions/geometry/interpolatedC2" },
				{ "$ref": "#/definitions/geometry/bezierSurfaceC0" },
				{ "$ref": "#/definitions/geometry/bezierSurfaceC2" }
				]
			}
		}
	},
	"additionalProperties": false,

			"definitions": {

		"normalizedValue": {
			"type": "number",
					"minimum": 0.0,
					"maximum": 1.0
		},

		"positiveValue": {
			"type": "number",
					"minimum": 0.0
		},

		"uint": {
			"type": "integer",
					"minimum": 0
		},


		"float2": {
			"type": "object",
					"properties" : {
				"x": { "type": "number" },
				"y": { "type": "number" }
			},
			"required": ["x", "y"],
			"additionalProperties": false
		},
		"float3": {
			"type": "object",
					"properties" : {
				"x": { "type": "number" },
				"y": { "type": "number" },
				"z": { "type": "number" }
			},
			"required": ["x", "y", "z"],
			"additionalProperties": false
		},

		"uint2": {
			"type": "object",
					"properties" : {
				"x": { "$ref": "#/definitions/uint" },
				"y": { "$ref": "#/definitions/uint" }
			},
			"required": ["x", "y"],
			"additionalProperties": false
		},

		"geometry": {

			"point": {
				"type": "object",
						"properties": {
					"id":	   { "$ref": "#/definitions/uint" },
					"name":	 { "type": "string" },
					"position": { "$ref": "#/definitions/float3" }
				},
				"required": ["id", "position"],
				"additionalProperties": false
			},

			"pointRef": {
				"type": "object",
						"properties": {
					"id": { "$ref": "#/definitions/uint" }
				},
				"required": ["id"],
				"additionalProperties": false
			},

			"controlPoints":	{
				"type": "array",
						"items": { "$ref": "#/definitions/geometry/pointRef" }
			},

			"patchControlPoints": {
				"type": "array",
						"items": { "$ref": "#/definitions/geometry/pointRef" },
				"minItems": 16,
						"maxItems": 16
			},

			"torus": {
				"type": "object",
						"properties": {
					"objectType":   { "const": "torus" },
					"id":		   { "$ref": "#/definitions/uint" },
					"name":		 { "type": "string" },
					"position":	 { "$ref": "#/definitions/float3" },
					"rotation":	 { "$ref": "#/definitions/float3" },
					"scale":		{ "$ref": "#/definitions/float3" },
					"samples":	  { "$ref": "#/definitions/uint2" },
					"smallRadius":  { "type": "number", "minimum": 0.0 },
					"largeRadius":  { "type": "number", "minimum": 0.0 }
				},
				"required": ["objectType", "id", "position", "rotation", "scale", "samples", "smallRadius", "largeRadius"],
				"additionalProperties": false
			},

			"bezierC0": {
				"type": "object",
						"properties": {
					"objectType":	   { "const": "bezierC0" },
					"id":			   { "$ref": "#/definitions/uint" },
					"name":			 { "type": "string" },
					"controlPoints":	{ "$ref": "#/definitions/geometry/controlPoints" }
				},
				"required": ["objectType", "id", "controlPoints"],
				"additionalProperties": false
			},

			"bezierC2": {
				"type": "object",
						"properties": {
					"objectType":	   { "const": "bezierC2" },
					"id":			   { "$ref": "#/definitions/uint" },
					"name":			 { "type": "string" },
					"deBoorPoints":	 { "$ref": "#/definitions/geometry/controlPoints" }
				},
				"required": ["objectType", "id", "deBoorPoints"],
				"additionalProperties": false
			},

			"interpolatedC2": {
				"type": "object",
						"properties": {
					"objectType":	   { "const": "interpolatedC2" },
					"id":			   { "$ref": "#/definitions/uint" },
					"name":			 { "type": "string" },
					"controlPoints":	{ "$ref": "#/definitions/geometry/controlPoints" }
				},
				"required": ["objectType", "id", "controlPoints"],
				"additionalProperties": false
			},

			"bezierPatchC0": {
				"type": "object",
						"properties": {
					"objectType":	   { "const": "bezierPatchC0" },
					"id":			   { "$ref": "#/definitions/uint" },
					"name":			 { "type": "string" },
					"controlPoints":	{ "$ref": "#/definitions/geometry/patchControlPoints" },
					"samples":		  { "$ref": "#/definitions/uint2" }
				},
				"required": ["objectType", "id", "controlPoints", "samples"],
				"additionalProperties": false
			},

			"bezierSurfaceC0": {
				"type": "object",
						"properties": {
					"objectType": { "const": "bezierSurfaceC0" },
					"id": { "$ref": "#/definitions/uint" },
					"name": { "type": "string" },
					"patches": {
						"type": "array",
								"items": { "$ref": "#/definitions/geometry/bezierPatchC0" }
					},
					"parameterWrapped": {
						"type": "object",
								"properties": {
							"u": { "type": "boolean" },
							"v": { "type": "boolean" }
						},
						"additionalProperties": false,
								"required": ["u", "v"]
					},
					"size": { "$ref": "#/definitions/uint2" }
				},
				"required": ["objectType", "id", "patches", "parameterWrapped", "size"],
				"additionalProperties": false
			},

			"bezierPatchC2": {
				"type": "object",
						"properties": {
					"objectType":	   { "const": "bezierPatchC2" },
					"id":			   { "$ref": "#/definitions/uint" },
					"name":			 { "type": "string" },
					"controlPoints":	{ "$ref": "#/definitions/geometry/patchControlPoints" },
					"samples":		  { "$ref": "#/definitions/uint2" }
				},
				"required": ["objectType", "id", "controlPoints", "samples"],
				"additionalProperties": false
			},

			"bezierSurfaceC2": {
				"type": "object",
						"properties": {
					"objectType":   { "const": "bezierSurfaceC2" },
					"id":		   { "$ref": "#/definitions/uint" },
					"name":		 { "type": "string" },
					"patches":	  {
						"type": "array",
								"items": { "$ref": "#/definitions/geometry/bezierPatchC2" }
					},
					"parameterWrapped": {
						"type": "object",
								"properties": {
							"u": { "type": "boolean" },
							"v": { "type": "boolean" }
						},
						"additionalProperties": false,
								"required": ["u", "v"]
					},
					"size": {
						"$ref": "#/definitions/uint2"
					}
				},
				"required": ["objectType", "id", "patches", "parameterWrapped", "size"],
				"additionalProperties": false
			}
		}
	}
})_JSON_";
#endif//MKWMG_SCHEMA_H
