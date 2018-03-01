{
    "targets": [
    	{
        	"target_name": "snakebot_native",
            "sources": [
            	"napi/index.cpp",
            	"napi/snakelib.cpp",
            	"napi/algorithms/cautious.cpp",
                "napi/algorithms/hungry.cpp",
                "napi/algorithms/termiantor.cpp",
                "napi/algorithms/random.cpp",
                "napi/algorithms/onedirection.cpp",
                "napi/algorithms/dog.cpp",
                "napi/algorithms/sim.cpp",
                "napi/algorithms/inyourface.cpp",
                "napi/test/testsuite.cpp",
                "napi/interop.cpp",
                "napi/algorithms.cpp",
                "napi/astar.cpp",
                "napi/movement.cpp",
                "napi/simulator.cpp",
                "napi/timing.cpp",
                "napi/benchmark/benchsuite.cpp"
            ],
            "cflags_cc": [
                "-std=c++17"
            ],
            "lflags": [],
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-rtti", "-fno-exceptions" ],
            "conditions": [
                [
                    'OS=="mac"',
                    { "xcode_settings": {
                        "GCC_ENABLE_CPP_RTTI": "YES",
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
                    } }
                ]
            ]
        }
    ]
}
