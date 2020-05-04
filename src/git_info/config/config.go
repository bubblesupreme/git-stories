package config

import (
	"encoding/json"
	"errors"
	"io/ioutil"
	"os"

	"../linter"

	log "github.com/sirupsen/logrus"
)

var lintersMap = map[string]func([]string) linter.ILinter{
	"cpplint": linter.CreateCPPLinter,
}

type User struct {
	GithubToken string `json:"github_token"`
	Owner       string `json:"owner"`
	Repository  string `json:"repository"`
}

type Linter struct {
	Name       string   `json:"name"`
	Parameters []string `json:"parameters"`
}

type Config struct {
	User    User     `json:"user"`
	Linters []Linter `json:"linters"`
}

type OutCommitInfo struct {
	Hash         string
	NewFiles     []string
	DeletedFiles []string
	ChangedFiles []string
	Errors       int
}

type OutConfig struct {
	Commits []OutCommitInfo
}

func ParseJsonConfig(filePath string) (*Config, error) {
	jsonFile, err := os.Open(filePath)
	if err != nil {
		log.WithFields(log.Fields{
			"filePath": filePath,
		}).Error("Failed to open file.")
		return nil, err
	}

	defer jsonFile.Close()

	byteValue, _ := ioutil.ReadAll(jsonFile)
	var config Config

	err = json.Unmarshal(byteValue, &config)
	if err != nil {
		log.WithFields(log.Fields{
			"filePath": filePath,
		}).Error("Failed to parse file.")
		return nil, err
	}

	return &config, nil
}

func (config *Config) GetUser() User {
	return config.User
}

func (config *Config) GetLinters() ([]linter.ILinter, error) {

	ILinters := make([]linter.ILinter, 0, len(config.Linters))
	for _, linter := range config.Linters {
		if f, ok := lintersMap[linter.Name]; ok {
			ILinters = append(ILinters, f(linter.Parameters))
		} else {
			log.WithFields(log.Fields{
				"linter": linter.Name,
			}).Error("Linter isn't supported")
		}
	}

	if len(ILinters) == 0 {
		return ILinters, errors.New("can't find supported linters")
	}

	return ILinters, nil
}

func (config *OutConfig) WriteResults(filePath string) error {
	resJson, err := json.Marshal(config)
	if err != nil {
		log.WithFields(log.Fields{
			"config": config,
		}).Error("Failed to encode struct OutConfig to JSON.")
		return err
	}

	err = ioutil.WriteFile(filePath, resJson, 0777)
	if err != nil {
		log.WithFields(log.Fields{
			"resJson": resJson,
		}).Error("Failed to write to file.")
		return err
	}

	return nil
}
