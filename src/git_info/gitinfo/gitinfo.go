package gitinfo

import (
	"errors"
	"fmt"
	"os"
	"path/filepath"

	"../config"

	git "github.com/go-git/go-git"
	"github.com/go-git/go-git/plumbing"
	"github.com/go-git/go-git/plumbing/object"
	"github.com/go-git/go-git/plumbing/transport/http"
	log "github.com/sirupsen/logrus"
)

type gitUser struct {
	client         http.BasicAuth
	owner          string
	repository     string
	repositoryPath string
}

func CreategitUser(user config.User) *gitUser {

	return &gitUser{
		client: http.BasicAuth{
			Username: user.Owner,
			Password: user.GithubToken,
		},
		owner:          user.Owner,
		repository:     user.Repository,
		repositoryPath: "",
	}
}

func (gitUser *gitUser) CloneRepository(workingDirectory string) (string, error) {
	url := gitUser.GetRepositoryHTTPSURL()
	repositoryPath := gitUser.getRepositoryPath(workingDirectory)
	var err error
	_, err = git.PlainClone(repositoryPath, false, &git.CloneOptions{
		Auth:     &gitUser.client,
		URL:      url,
		Progress: os.Stdout,
	})
	if err != nil {
		log.WithFields(log.Fields{
			"repositoryPath": repositoryPath,
			"url":            url,
		}).Error("Failed to clone repository.")

		return "", err
	}

	gitUser.repositoryPath = repositoryPath
	return gitUser.repositoryPath, nil
}

func (gitUser *gitUser) GetCommits() ([]*object.Commit, error) {
	commits := make([]*object.Commit, 0, 20)

	rep, err := gitUser.getRepository()
	if err != nil {
		log.WithFields(log.Fields{
			"repositoryPath": gitUser.repositoryPath,
		}).Error("Failed to get repository by path.")

		return nil, err
	}

	head, err := gitUser.getRepositoryHead()
	if err != nil {
		log.Error("Failed to get repository head.")
		return commits, err
	}

	cIter, err := rep.Log(&git.LogOptions{From: head.Hash()})
	if err != nil {
		log.WithFields(log.Fields{
			"repository": rep,
		}).Error("Failed to get repository commits.")

		return commits, err
	}

	err = cIter.ForEach(func(c *object.Commit) error {
		commits = append(commits, c)

		return nil
	})
	if err != nil {
		log.WithFields(log.Fields{
			"cIter": cIter,
		}).Error("Failed to iterate by commits.")

		return commits, err
	}

	return commits, nil
}

func (gitUser *gitUser) Checkout(commit *object.Commit) error {
	workTree, err := gitUser.getRepositoryWorkTree()

	if err != nil {
		log.Error("Failed to get repository work tree.")
		return err
	}

	return workTree.Checkout(&git.CheckoutOptions{
		Hash: commit.Hash,
	})
}

func (gitUser *gitUser) getRepository() (*git.Repository, error) {
	if gitUser.repositoryPath == "" {
		return nil, errors.New("pre-clone repository is needed")
	}

	return git.PlainOpen(gitUser.repositoryPath)
}

func (gitUser *gitUser) getRepositoryHead() (*plumbing.Reference, error) {
	rep, err := gitUser.getRepository()
	if err != nil {
		log.WithFields(log.Fields{
			"repositoryPath": gitUser.repositoryPath,
		}).Error("Failed to get repository by path.")

		return nil, err
	}

	return rep.Head()
}

func (gitUser *gitUser) getRepositoryWorkTree() (*git.Worktree, error) {
	rep, err := gitUser.getRepository()

	if err != nil {
		log.Error("Failed to get repository head.")
		return nil, err
	}

	return rep.Worktree()
}

func (gitUser *gitUser) GetRepositoryHTTPSURL() string {
	return "https://github.com/" + gitUser.owner + "/" + gitUser.repository + ".git"
}

func (gitUser *gitUser) getRepositoryPath(workingDirectory string) string {
	return filepath.Join(workingDirectory, gitUser.repository)
}

func (gitUser *gitUser) GetCommitInfo(commit *object.Commit) ([]string, []string, []string, error) {
	newFiles := make([]string, 0, 20)
	deletedFiles := make([]string, 0, 20)
	changedFiles := make([]string, 0, 20)

	fmt.Println(len(commit.ParentHashes))
	parent, err := commit.Parent(0)
	if err == object.ErrParentNotFound {
		log.WithFields(log.Fields{
			"commit": commit,
		}).Info("First commit.")
		tree, err := commit.Tree()
		if err != nil {
			log.WithFields(log.Fields{
				"commit": commit,
			}).Error("Failed to get tree from a commit.")
			return newFiles, deletedFiles, changedFiles, err
		}

		tree.Files().ForEach(func(file *object.File) error {
			newFiles = append(newFiles, file.Name)
			return nil
		})

		return newFiles, deletedFiles, changedFiles, nil
	}

	patch, err := parent.Patch(commit)
	if err != nil {
		log.WithFields(log.Fields{
			"commit": commit,
		}).Error("Failed to get a patch from a commit.")

		return newFiles, deletedFiles, changedFiles, err
	}

	for _, filePatches := range patch.FilePatches() {
		from, to := filePatches.Files()
		if from == nil {
			fmt.Println("New File is created")
			newFiles = append(newFiles, to.Path())
		} else if to == nil {
			fmt.Println("File is deleted")
			deletedFiles = append(deletedFiles, from.Path())
		} else if from.Path() != to.Path() {
			fmt.Println("File is renamed")
			newFiles = append(newFiles, to.Path())
			deletedFiles = append(deletedFiles, from.Path())
		} else {
			fmt.Println("File is changed")
			changedFiles = append(changedFiles, from.Path())
		}
	}

	return newFiles, deletedFiles, changedFiles, nil
}