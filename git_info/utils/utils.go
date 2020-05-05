package utils

import (
	"errors"
	"os"
	"os/exec"
	"path/filepath"

	log "github.com/sirupsen/logrus"
)

func GetFullPath(name string) (string, error) {
	if filepath.IsAbs(name) {
		return name, nil
	}
	return filepath.Abs(filepath.Base(name))
}

func IsPathExists(path string) (bool, error) {
	_, err := os.Stat(path)
	if err != nil {
		if os.IsNotExist(err) {
			return false, nil
		} else {
			log.WithFields(log.Fields{
				"path": path,
			}).Error("failed to get file or directory status.")
			return true, err
		}
	}
	return true, nil
}

func CreateFolder(folderName string, removeIfExists bool) error {
	if folderName == "" {
		errors.New("folder name is empty.")
	}

	fullPath, err := GetFullPath(folderName)
	isExists, err := IsPathExists(fullPath)
	if err != nil {
		log.WithFields(log.Fields{
			"fullPath": fullPath,
		}).Error("Failed to check path existing.")
		return err
	}

	if isExists {
		if removeIfExists {
			log.WithFields(log.Fields{
				"fullPath": fullPath,
			}).Warning("Path is already exists, so remove and recreate it.")

			err = RemoveFolder(fullPath, true)
			if err != nil {
				log.WithFields(log.Fields{
					"fullPath": fullPath,
				}).Error("Failed to remove folder.")
				return err
			}
		} else {
			errors.New("path is already exists")
		}
	}

	if err != nil {
		log.WithFields(log.Fields{
			"folderName": folderName,
		}).Error("Failed to get full path.")

		return err
	}

	err = os.Mkdir(fullPath, 0777)
	if err != nil {
		log.WithFields(log.Fields{
			"path": fullPath,
		}).Error("Failed to create folder.")

		return err
	}

	return nil
}

func RemoveFolder(folderName string, recursive bool) error {
	if folderName == "" {
		errors.New("folder name is empty.")
	}

	fullPath, err := GetFullPath(folderName)
	if err != nil {
		log.WithFields(log.Fields{
			"folderName": folderName,
		}).Error("Failed to get full path.")

		return err
	}

	if recursive {
		err = os.RemoveAll(fullPath)
	} else {
		err = os.Remove(fullPath)
	}

	if err != nil {
		log.WithFields(log.Fields{
			"fullPath":  fullPath,
			"recursive": recursive,
		}).Error("Failed to remove folder.")

		return err
	}

	return nil
}

func RunCommand(command []string, dir string) ([]byte, error) {
	cmd := exec.Command(command[0], command[1:]...)
	if dir != "" {
		cmd.Dir = dir
	}
	return cmd.CombinedOutput()
}
