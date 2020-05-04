package worker

import (
	"fmt"
	"path/filepath"

	"../config"
	"../gitinfo"
	"../linter"
	"../utils"

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

func (worker *Worker) Work(configPath string) error {

	err := utils.CreateFolder(worker.workingFolderFullPath, true)
	if err != nil {
		log.WithFields(log.Fields{
			"workingFolderFullPath": worker.workingFolderFullPath,
		}).Error("Failed to create folder.")
		return err
	}

	userConfig, err := config.ParseJsonConfig(configPath)
	if err != nil {
		log.WithFields(log.Fields{
			"configPath": configPath,
		}).Error("Failed to parse config.")
		return err
	}
	user := userConfig.GetUser()
	linters, err := userConfig.GetLinters()
	if err != nil {
		log.WithFields(log.Fields{
			"linter": userConfig.Linters,
		}).Error("Failed to get linter.")

		return err
	}
	gitUser := gitinfo.CreategitUser(user)
	url := gitUser.GetRepositoryHTTPSURL()

	_, err = gitUser.CloneRepository(worker.workingFolderFullPath)
	if err != nil {
		log.WithFields(log.Fields{
			"url": url,
		}).Error("Failed to clone repository.")

		return err
	}

	commits, err := gitUser.GetCommits()
	if err != nil {
		log.Error("Failed to get commits.")
		return err
	}
	fmt.Println(commits)

	outConfig := config.OutConfig{
		Commits: make([]config.OutCommitInfo, 0, len(commits)),
	}

	var commonRes int
	// reversed range
	for i := len(commits) - 1; i >= 0; i-- {
		err = gitUser.Checkout(commits[i])
		if err != nil {
			log.WithFields(log.Fields{
				"commit": commits[i],
			}).Error("Failed to checkout.")
			return err
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
				return err
			}

			var res int
			if err == linter.NoFilesError() {
				res = 0
			} else {
				res, err = lint.CalculateResult(numErrors)
				if err != nil {
					log.Error("Failed to calculate result.")
					return err
				}
			}
			commonRes = res
		}

		fmt.Println(commonRes)
		newFiles, deletedFiles, changedFiles, err := gitUser.GetCommitInfo(commits[i])
		if err != nil {
			log.WithFields(log.Fields{
				"commit": commits[i],
			}).Error("Failed to get commit information.")
			return err
		}
		outConfig.Commits = append(outConfig.Commits, config.OutCommitInfo{
			Hash:         commits[i].Hash.String(),
			NewFiles:     newFiles,
			DeletedFiles: deletedFiles,
			ChangedFiles: changedFiles,
			Errors:       commonRes,
		})
	}

	err = outConfig.WriteResults(filepath.Join(worker.workingFolderFullPath, "output.json"))
	if err != nil {
		log.WithFields(log.Fields{
			"outConfig": outConfig,
		}).Error("Failed to write results.")
		return err
	}

	// err = utils.RemoveFolder(worker.workingFolderFullPath, true)
	// if err != nil {
	// 	log.WithFields(log.Fields{
	// 		"workingFolderFullPath": worker.workingFolderFullPath,
	// 	}).Error("Failed to remove folder.")
	// 	return err
	// }
	return nil
}
