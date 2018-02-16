{
	"targets": [
    	{
        	"target_name": "snakebot_native",
            "sources": [
            	"napi/index.cpp",
            	"napi/snakelib.cpp",
            	"napi/algorithms/cautious.cpp",
                "napi/algorithms/hungry.cpp",
                "napi/algorithms/dog.cpp",
                "napi/algorithms/sim.cpp",
                "napi/test/testsuite.cpp",
                "napi/interop.cpp",
                "napi/astar.cpp",
                "napi/movement.cpp",
                "napi/simulator.cpp",
                "napi/timing.cpp",
                "napi/benchmark/benchsuite.cpp"
            ],
            "cflags": [
                "-std=c++17"
            ],
            "lflags": [],
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