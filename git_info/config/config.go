// Copyright © 2020 Svetlana Emelianova
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

package config

import (
	"encoding/json"
	"errors"
	"io/ioutil"
	"os"

	"github.com/bubblesupreme/git-stories/git_info/linter"

	"google.golang.org/protobuf/proto"

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
	file, err := os.Create(filePath)
	if err != nil {
		log.WithFields(log.Fields{
			"filePath": filePath,
		}).Error("Failed to create file.")
		return err
	}

	data, err := proto.Marshal(config)
	if err != nil {
		log.WithFields(log.Fields{
			"config": config,
		}).Error("Failed to encode struct OutConfig to bytes.")
		return err
	}

	_, err = file.Write(data)
	if err != nil {
		log.WithFields(log.Fields{
			"file": file,
		}).Error("Failed to write bytes to file.")
		return err
	}

	// fmt.Println(data)

	return nil
}
