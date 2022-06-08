#include "headers.h"

std::string getModulePath()
{
	py::object stratega = py::module::import("stratega");
	return stratega.attr("__file__").cast<std::string>();
}

bool isLocalResourcesPath(std::string& modulePath, std::string& configPath)
{
	//Check if config exist	
	ghc::filesystem::path newPossiblePath(modulePath);
	newPossiblePath = newPossiblePath.parent_path() / configPath;

	if (ghc::filesystem::exists(newPossiblePath))
		return true;
	else
		return false;
}

std::unique_ptr<SGA::GameConfig> loadConfig(std::string& path)
{
	auto modulePath = getModulePath();
	ghc::filesystem::path modulePythonPath(modulePath);

	modulePythonPath.make_preferred();
	ghc::filesystem::path pathCheck(path);

	if (pathCheck.is_absolute())
	{
		auto resourcePath = (modulePythonPath.parent_path() / "resources").string();
		auto config = SGA::loadConfigFromYAML(path, (modulePythonPath.parent_path() / "resources").string());
		return std::move(config);
	}
	else
	{
		if (isLocalResourcesPath(modulePath, path))
		{
			auto newPossiblePath = modulePythonPath.parent_path() / path;
			auto config = SGA::loadConfigFromYAML(newPossiblePath.string(), (modulePythonPath.parent_path() / "resources").string());
			return std::move(config);
		}
		else
		{
			auto config = SGA::loadConfigFromYAML(path, (modulePythonPath.parent_path() / "resources").string());
			return std::move(config);
		}
	}
}

std::unique_ptr<SGA::GameRunner> createRunner(const SGA::GameConfig* gameConfig)
{
	return std::move(SGA::createGameRunner(*gameConfig));
}

std::vector<std::shared_ptr<SGA::Agent>> generateAgents(const SGA::GameConfig* gameConfig)
{
	auto agents = gameConfig->generateAgents();
	std::vector<std::shared_ptr<SGA::Agent>> newAgents;

	std::move(agents.begin(), agents.end(), std::back_inserter(newAgents));

	return newAgents;
}

void setDefaultLogger(std::string logPath)
{
	SGA::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
}

Arena createArena(const SGA::GameConfig* gameConfig)
{
	return Arena(*gameConfig);
}

namespace stratega
{
	void default_methods(py::module_& m)
	{
		m.def("load_config", &loadConfig, "Loads game config", py::arg("path"), py::call_guard<py::scoped_ostream_redirect,
			py::scoped_estream_redirect>());
		m.def("create_runner", &createRunner, "Create game runner", py::arg("gameConfig"));
		m.def("create_arena", &createArena, "Create game aren", py::arg("gameConfig"));
		m.def("generate_agents", &generateAgents, "Generate agents", py::arg("gameConfig"));
		m.def("set_default_logger", &setDefaultLogger, "Set default logger", py::arg("logPath"));
		m.def("load_levels_from_yaml", &SGA::loadLevelsFromYAML, "Load Levels definitions  from YAML", py::arg("fileMapsPath"), py::arg("config"));
	}	
}