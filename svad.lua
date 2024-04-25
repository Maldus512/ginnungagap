#!/usr/bin/env lua

-- [...] he asked that they would give him leave to have the help of his stallion, which was called Svadilfari;
-- [...] and it seemed very marvellous to the Æsir what great rocks that horse drew, for the horse did more rough work by half than did the wright.

local TOOLCHAIN = "arm-none-eabi"
local PROJECT = "ginnungagap"
local BUILD_FOLDER = "build"

local COMPILER = TOOLCHAIN .. "-gcc"
local LINKER = TOOLCHAIN .. "-ld"
local OBJCOPY = TOOLCHAIN .. "-objcopy"
local GDB = TOOLCHAIN .. "-gdb"

local INCLUDES = "-Icomponents -Icomponents/pico/boot_stage2/picosdk/"
local MAIN_MEMMAP = "main/memmap.ld"
local FCPU = "-mcpu=cortex-m0"
local CFLAGS = "-nostartfiles -ffreestanding -O0 -g -mthumb -Wall -Wextra -pedantic " .. FCPU .. " " .. INCLUDES
local LFLAGS = "-nostdlib"

local COMPONENT_RP2040 = "components/rp2040"
local COMPONENT_PICO = "components/pico"
local COMPONENT_BOOT_STAGE2 = COMPONENT_PICO .. "/boot_stage2"

local BOOT_STAGE2_MEMMAP = COMPONENT_BOOT_STAGE2 .. "/memmap.ld"

local ELF = BUILD_FOLDER .. "/" .. PROJECT .. ".elf"

local NINJA_FILE = string.format("%s/build.ninja", BUILD_FOLDER)


local runWithOutput = function(command)
    local output = io.popen(command)
    if output ~= nil then
        local line = output:read()
        while line ~= nil do
            print(line)
            line = output:read()
        end
        local _, _, code = output:close()
        return code == 0
    else
        return false
    end
end

local clean = function()
    if runWithOutput(string.format("ninja -f %s -t clean", NINJA_FILE)) then
        print("Clean successful")
    else
        print("Clean failed!")
    end
end

local configure = function()
    local svad = require "tools.lua.svadilfari"

    print("Configuring build...")

    ---@class CNinja : Ninja
    ---@field compileFile fun(self: CNinja, args: {rule: string, input: string}): string
    ---@field compileC fun(self: CNinja, input: string): string
    local ninja = svad.openNinja(BUILD_FOLDER)

    ninja.compileFile = function(self, args)
        return self:build {
            rule = args.rule,
            input = args.input,
            output = svad.toExtension("o"),
        }
    end
    ninja.compileC = function(self, input)
        return self:compileFile {
            rule = "gcc",
            input = input,
        }
    end

    ninja:rule {
        name = "gcc",
        command = COMPILER .. " -MD -MF $out.d " .. CFLAGS .. " -c $in -o $out",
        env = { deps = "gcc", depfile = "$out.d" },
    }
    ninja:rule {
        name = "link",
        command = LINKER .. " " .. LFLAGS .. " $extraFlags " .. " $in -o $out",
    }
    ninja:rule {
        name = "objcopy-binary",
        command = OBJCOPY .. " -O binary $in $out",
    }
    ninja:rule {
        name = "patch-boot-stage2",
        command = "./svad.lua stage2 $in $out"
    }
    ninja:rule {
        name = "uf2",
        command = "./svad.lua uf2 $in $out"
    }
    ninja:rule {
        name = "reconfigure",
        command = "./svad.lua configure"
    }


    -- Build Stage 2 bootloader
    local boot_stage2_objects = ""

    --[[
    boot_stage2_objects = boot_stage2_objects ..
        " " .. ninja:compileC(COMPONENT_BOOT_STAGE2 .. "/boot_stage2.c")
    boot_stage2_objects = boot_stage2_objects ..
        " " .. ninja:compileC(COMPONENT_BOOT_STAGE2 .. "/start.S")
        ]]
    --boot_stage2_objects = boot_stage2_objects .. " " .. ninja:compileC(COMPONENT_BOOT_STAGE2 .. "/picosdk/boot2_w25q080.S")
    boot_stage2_objects = boot_stage2_objects .. " " .. ninja:compileC(COMPONENT_BOOT_STAGE2 .. "/boot_stage2.c")

    local boot_stage2_checked_object = ninja:pipe {
        {
            rule = "link",
            input = boot_stage2_objects,
            output = COMPONENT_BOOT_STAGE2 .. "/boot_stage2.elf",
            implicitDependencies = { BOOT_STAGE2_MEMMAP },
            env = { extraFlags = "-T " .. BOOT_STAGE2_MEMMAP },
        },
        {
            rule = "objcopy-binary",
            output = svad.toExtension("bin"),
        },
        {
            rule = "patch-boot-stage2",
            output = function(input) return input:gsub("%.bin$", "_checked.s") end,
        },
        {
            rule = "gcc",
            output = svad.toExtension("o"),
        }
    }

    -- Build Main application
    local objects = boot_stage2_checked_object

    for _, source in pairs(svad.listFilesOfType("main", "c")) do
        objects = objects .. " " .. ninja:compileC(source)
    end

    for _, source in pairs(svad.listFilesOfType("components", "c", { COMPONENT_BOOT_STAGE2 })) do
        objects = objects .. " " .. ninja:compileC(source)
    end

    ninja:pipe {
        {
            rule = "link",
            input = objects,
            output = ELF,
            implicitDependencies = { MAIN_MEMMAP },
            env = { extraFlags = "-T " .. MAIN_MEMMAP },
        },
        {
            rule = "objcopy-binary",
            output = svad.toExtension("bin"),
        },
        {
            rule = "uf2",
            output = svad.toExtension("uf2"),
        },
    }

    ninja:build {
        rule = "reconfigure",
        input = "svad.lua",
        output = "build.ninja",
        env = { generator = "true" }
    }

    ninja:close()

    io.popen(string.format("ninja -f %s -t compdb > compile_commands.json", NINJA_FILE))

    print("Build ready!")
end

local build = function()
    local file = io.open(NINJA_FILE)
    if file == nil then
        configure()
    else
        file:close()
    end

    if runWithOutput("ninja -f " .. NINJA_FILE) then
        print("Build successful")
        return true
    else
        print("Build failed!")
        return false
    end
end

local gdbrun = function()
    if build() and runWithOutput(GDB .. " -x tools/debug/load-and-exit.gdb " .. ELF) then
        print("Binary loaded to RAM")
    else
        print("Binary load failed!")
    end
end


local stage2 = function(inputPath, outputPath)
    local postProcessing = require "tools.lua.postProcessing"
    local inputFile = io.open(inputPath, "rb")

    if inputFile == nil then
        print("Unable to open " .. inputPath)
        return
    end

    local contents = inputFile:read("a")
    inputFile:close()

    if contents == nil then
        print("Unable to read " .. inputPath)
        return
    end

    print("Input len " .. string.format("%i", #contents))
    local bytes = postProcessing.padTo({ string.byte(contents, 1, #contents) }, 252)
    local checksum = postProcessing.crc32(bytes)

    for _, byte in pairs(checksum) do
        table.insert(bytes, byte)
    end

    local outputFile = io.open(outputPath, "wb")

    if outputFile == nil then
        print("Unable to open " .. outputPath)
        return
    end

    outputFile:write("// Padded and checksummed version of: " .. inputPath .. "\n\n")
    outputFile:write(".cpu cortex-m0plus\n")
    outputFile:write(".thumb\n\n")
    outputFile:write(".section .boot2, \"ax\"\n\n")

    --[[
    local offset = 0
    while offset * 16 < #bytes do
        local chunk = table.move(bytes, offset * 16 + 1, (offset + 1) * 16, 1, {})
        local hexBytes = {}
        for _, byte in pairs(chunk) do
            table.insert(hexBytes, string.format("0x%02x", byte))
        end

        outputFile:write(".byte " .. table.concat(hexBytes, ", ") .. "\n")
        offset = offset + 1
    end
    ]]
    --

    for _, byte in pairs(bytes) do
        outputFile:write(string.format(".byte 0x%02X\n", byte))
    end

    outputFile:close()
end


local uf2 = function(inputPath, outputPath)
    local postProcessing = require "tools.lua.postProcessing"
    local inputFile = io.open(inputPath, "rb")

    if inputFile == nil then
        print("Unable to open " .. inputPath)
        return
    end

    local contents = inputFile:read("a")
    inputFile:close()

    if contents == nil then
        print("Unable to read " .. inputPath)
        return
    end

    local uft2Data = postProcessing.uf2({ string.byte(contents, 1, #contents) })
    local outputFile = io.open(outputPath, "wb")

    if outputFile == nil then
        print("Unable to open " .. outputPath)
        return
    end

    outputFile:write(string.char(table.unpack(uft2Data)))
    outputFile:close()
end

local commands = {
    {
        name = "configure",
        description = "Configure the build",
        action = configure,
    },
    {
        name = "build",
        description = "Run the build",
        action = build,
    },
    {
        name = "stage2",
        description = "Prepare the stage2 bootloader",
        action = stage2,
    },
    {
        name = "uf2",
        description = "Prepare the uf2 packet",
        action = uf2,
    },
    {
        name = "clean",
        description = "Clean intermediate build files",
        action = clean,
    },
    {
        name = "gdbrun",
        description = "Load and run the executable with GDB",
        action = gdbrun,
    },
    {
        name = "gdb",
        description = "Starts a GDB debugging session",
        action = function()
            build()
            os.execute(GDB .. " -x tools/debug/load-and-stop.gdb " .. ELF)
        end
    },
    {
        name = "openocd",
        description = "Run the openocd server",
        action = function()
            os.execute(
                [[openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -f tools/debug/openocd-gdb.config -c "adapter speed 5000"]])
        end
    }
}


local printHelp = function()
    print("Usage:")
    print("  svad.lua [command]")
    print("")
    print("COMMANDS")
    for _, command in pairs(commands) do
        print(string.format("  %-15s%s", command.name, command.description))
    end
end


local main = function()
    if #arg < 1 or arg[1] == "help" then
        print(
            "Svaðilfari, tireless horse that (almost) built the walls of Asgard is now at our service to build some software - with the help of a mercenary from feudal Japan, apparently.")
        print("")
        printHelp()
    else
        for _, command in pairs(commands) do
            if command.name == arg[1] then
                command.action(table.unpack(arg, 2))
                return
            end
        end

        print("Unrecognized command: " .. arg[1])
        print("")
        printHelp()
    end
end


main()
