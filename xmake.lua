add_rules("mode.release")

target("toy")
    set_kind("binary")
    set_optimize("fastest")
    set_toolchains("clang")
    add_cxxflags("-DSNAP", {force = true})
    add_files("src/model/*.cpp")
    add_files("src/utils/*.cpp")
    on_load(function (target)
        root_args = os.iorun(
            "root-config --cflags --glibs --libs"
        ):split(" ")
        table.insert(root_args, "-lMathMore")
        table.insert(root_args, "-lRooFit")

        threads = os.cpuinfo().ncpu
        table.insert(root_args, "-DNTHREADS=" .. threads)

        wd = os.getenv("PWD") -- only works on unix systems
        table.insert(root_args, "-DPROJECT_DIR=\"" .. wd .. "\"")

        table.insert(root_args, {force = true})

        target:add("cxxflags", unpack(root_args))
        target:add("ldflags", unpack(root_args))
    end)

target("an")
    set_kind("binary")
    set_optimize("fastest")
    set_toolchains("clang")
    add_files("src/analysis/*.cpp")
    add_files("src/utils/files.cpp")
    on_load(function (target)
        root_args = os.iorun(
            "root-config --cflags --glibs --libs"
        ):split(" ")
        table.insert(root_args, "-lTMVA")
        table.insert(root_args, "-lXMLIO") -- needed for RReader apparently

        threads = os.cpuinfo().ncpu
        table.insert(root_args, "-DNTHREADS=" .. threads)

        wd = os.getenv("PWD") -- only works on unix systems
        table.insert(root_args, "-DPROJECT_DIR=\"" .. wd .. "\"")

        table.insert(root_args, {force = true})

        target:add("cxxflags", unpack(root_args))
        target:add("ldflags", unpack(root_args))
    end)