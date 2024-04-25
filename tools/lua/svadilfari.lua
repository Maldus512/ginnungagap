local dirname = function(path)
    return path:gsub("/[^/]*$", "")
end

local toExtension = function(extension)
    return function(input)
        return input:gsub("%.[^.]+$", "." .. extension)
    end
end

local containsDirectory = function(array, directory)
    for _, element in pairs(array) do
        if dirname(element) == dirname(directory) then
            return true
        end
    end
    return false
end

---@param path string
---@param extension string
---@param exclude string[] | nil
---@return string[]
local listFilesOfType = function(path, extension, exclude)
    exclude = exclude or {}
    local files = {}

    for file in io.popen("find " .. path .. " -name *." .. extension):lines() do
        if not containsDirectory(exclude, dirname(file)) then
            table.insert(files, file)
        end
    end

    return files
end

---@param buildFolder string
---@return Ninja
local openNinja = function(buildFolder)
    if buildFolder == nil then
        buildFolder = "."
    else
        os.execute("mkdir -p " .. buildFolder)
    end

    local ninjaFile = io.open(buildFolder .. "/build.ninja", "w")

    local addEnvironment = function(file, environment)
        for key, value in pairs(environment) do
            file:write("    " .. key .. " = " .. value .. "\n")
        end
    end

    ---@alias BuildConfig { rule: string, input?: string, output: (string | fun(string): string), implicitDependencies: table?, env: table? }

    ---@class Ninja
    ---@field file file*?
    ---@field buildFolder string
    ---@field rule fun(self: Ninja, args: { name: string, command: string, env: table? }): nil
    ---@field build fun(self: Ninja, args: BuildConfig): string
    ---@field pipe fun(self: Ninja, args: BuildConfig[]): string
    ---@field close fun(self: Ninja): nil

    ---@type fun(ninja: Ninja, args: BuildConfig, toBuildFolder: boolean): string
    local buildRule = function(ninja, args, toBuildFolder)
        local output = args.output
        if type(args.output) == "function" then
            output = args.output(args.input)
        end
        if toBuildFolder then
            output = ninja.buildFolder .. "/" .. output
        end

        local build = string.format("build %s: %s %s", output, args.rule, args.input)
        if args.implicitDependencies ~= nil then
            build = build .. " |"
            for _, dep in pairs(args.implicitDependencies) do
                build = build .. " " .. dep
            end
        end
        build = build .. "\n"

        ninja.file:write(build)

        if args.env ~= nil then
            addEnvironment(ninja.file, args.env)
        end
        ninja.file:write("\n")

        return output --[[@as string]]
    end

    ---@type Ninja
    return {
        file = ninjaFile,
        buildFolder = buildFolder,
        rule = function(self, args)
            self.file:write("rule " .. args.name .. "\n    command = " .. args.command .. "\n")
            if args.env ~= nil then
                addEnvironment(self.file, args.env)
            end
            self.file:write("\n")
        end,
        build = function(self, args)
            return buildRule(self, args, true)
        end,
        close = function(self)
            self.file:close()
        end,
        pipe = function(self, args)
            local input = nil

            for _, buildStep in ipairs(args) do
                if buildStep.input == nil then
                    buildStep.input = input
                end

                input = buildRule(self, buildStep, false)
            end

            return input --[[@as string]]
        end
    }
end


return {
    openNinja = openNinja,
    listFilesOfType = listFilesOfType,
    toExtension = toExtension,
}
