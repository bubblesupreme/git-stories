package linter

import (
	"errors"

	"../utils"
)

var noFilesError = errors.New("there are no files matching this linter")

type ILinter interface {
	ParseOutput(output []byte, err error) (int, error)
	CalculateResult(errors int) (int, error)
	Run(directory string) ([]byte, error)
	CheckError(err error) error
}

type Linter struct {
	name           string
	parameters     []string
	resultTemplate []byte
}

func (linter *Linter) GetName() string {
	return linter.name
}

func (linter *Linter) GetParameters() []string {
	return linter.parameters
}

func (linter *Linter) Run(directory string) ([]byte, error) {
	command := []string{linter.name}
	command = append(command, linter.parameters...)
	return utils.RunCommand(command, directory)
}

func NoFilesError() error {
	return noFilesError
}
