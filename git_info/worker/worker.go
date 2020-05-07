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

package worker

import (
	"path/filepath"

	"github.com/bubblesupreme/git-stories/git_info/config"
	"github.com/bubblesupreme/git-stories/git_info/gitinfo"
	"github.com/bubblesupreme/git-stories/git_info/linter"
	"github.com/bubblesupreme/git-stories/git_info/utils"

	log "github.com/sirupsen/logrus"
)

const (
	workingFolderName = "Temp"
)

func getCloneRepositoryString(url string) string {
	return "git clone " + url
}

type Worker struct {
	workingFolderFullPath string
}

func CreateWorker() (*Worker, error) {
	workingFolderFullPath, err := utils.GetFullPath(workingFolderName)
	if err != nil {
		log.WithFields(log.Fields{
			"workingFolderName": workingFolderName,
		}).Error("Failed to get folder full path.")
		return nil, err
	}

	return &Worker{
		workingFolderFullPath: workingFolderFullPath,
	}, nil
}

func (worker *Worker) createResultConfig(configPath string) (string, error) {
	userConfig, err := config.ParseJsonConfig(configPath)
	if err != nil {
		log.WithFields(log.Fields{
			"configPath": configPath,
		}).Error("Failed to parse config.")
		return "", err
	}
	user := userConfig.GetUser()
	linters, err := userConfig.GetLinters()
	if err != nil {
		log.WithFields(log.Fields{
			"linter": userConfig.Linters,
		}).Error("Failed to get linter.")

		return "", err
	}
	gitUser := gitinfo.CreategitUser(user)
	url := gitUser.GetRepositoryHTTPSURL()

	_, err = gitUser.CloneRepository(worker.workingFolderFullPath)
	if err != nil {
		log.WithFields(log.Fields{
			"url": url,
		}).Error("Failed to clone repository.")

		return "", err
	}

	commits, err := gitUser.GetCommits()
	if err != nil {
		log.Error("Failed to get commits.")
		return "", err
	}
	// fmt.Println(commits)

	outConfig := config.OutConfig{
		Commits: make([]*config.CommitInfo, 0, len(commits)),
	}

	var commonRes int
	// reversed range
	for i := len(commits) - 1; i >= 0; i-- {
		err = gitUser.Checkout(commits[i])
		if err != nil {
			log.WithFields(log.Fields{
				"commit": commits[i],
			}).Error("Failed to checkout.")
			return "", err
		}

		for _, lint := range linters {
			output, err := lint.Run(worker.workingFolderFullPath)
			// fmt.Println(string(output))
			// fmt.Println(err)

			numErrors, err := lint.ParseOutput(output, err)

			if err != nil && err != linter.NoFilesError() {
				log.WithFields(log.Fields{
					"output": string(output),
				}).Error("Failed to parse output.")
				return "", err
			}

			var res int
			if err == linter.NoFilesError() {
				res = 0
			} else {
				res, err = lint.CalculateResult(numErrors)
				if err != nil {
					log.Error("Failed to calculate result.")
					return "", err
				}
			}
			commonRes += res
		}

		// fmt.Println(commonRes)
		newFiles, deletedFiles, changedFiles, err := gitUser.GetCommitInfo(commits[i])
		if err != nil {
			log.WithFields(log.Fields{
				"commit": commits[i],
			}).Error("Failed to get commit information.")
			return "", err
		}
		outConfig.Commits = append(outConfig.Commits, &config.CommitInfo{
			Hash:         commits[i].Hash.String(),
			NewFiles:     newFiles,
			DeletedFiles: deletedFiles,
			ChangedFiles: changedFiles,
			Errors:       int32(commonRes),
		})
	}

	resultFile := filepath.Join(worker.workingFolderFullPath, "output.gs")
	err = outConfig.WriteResults(resultFile)
	if err != nil {
		log.WithFields(log.Fields{
			"outConfig": outConfig,
		}).Error("Failed to write results.")
		return "", err
	}

	return resultFile, nil
}

func (worker *Worker) Work(configPath string) error {

	err := utils.CreateFolder(worker.workingFolderFullPath, true)
	if err != nil {
		log.WithFields(log.Fields{
			"workingFolderFullPath": worker.workingFolderFullPath,
		}).Error("Failed to create folder.")
		return err
	}

	resultFile, err := worker.createResultConfig(configPath)
	if err != nil {
		log.Error("Failed to create result config.")
		return err
	}

	command := []string{"./gs_rendering", resultFile}
	output, err := utils.RunCommand(command, "")
	if err != nil {
		log.WithFields(log.Fields{
			"command": command,
			"output":  string(output),
		}).Error("Failed to run command.")
		return err
	}

	err = utils.RemoveFolder(worker.workingFolderFullPath, true)
	if err != nil {
		log.WithFields(log.Fields{
			"workingFolderFullPath": worker.workingFolderFullPath,
		}).Error("Failed to remove folder.")
		return err
	}
	return nil
}

func (worker *Worker) ErrorCleanup() error {
	if worker.workingFolderFullPath != "" {
		isExists, _ := utils.IsPathExists(worker.workingFolderFullPath)
		if isExists {
			err := utils.RemoveFolder(worker.workingFolderFullPath, true)
			if err != nil {
				log.WithFields(log.Fields{
					"workingFolderFullPath": worker.workingFolderFullPath,
				}).Error("Failed to remove folder.")
				return err
			}
		}
	}
	return nil
}
