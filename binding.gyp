{
	"targets": [
    	{
        	"target_name": "snakebot_native",
            "sources": [
            	"napi/index.cpp",
            	"napi/snakelib.cpp",
            	"napi/algorithms/test.cpp",
                "napi/test/testsuite.cpp",
                "napi/interop.cpp",
                "napi/astar.cpp",
                "napi/movement.cpp",
                "napi/simulator.cpp"
            ],
            "cflags": [
                "-std=c++17"
            ],
            "cflags_cc!": [ "-fno-rtti" ],
            "conditions": [
                [
                    'OS=="mac"',
                    { "xcode_settings": { "GCC_ENABLE_CPP_RTTI": "YES" } }
                ]
            ]
        }
    ]
}