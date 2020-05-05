package linter

import (
	"bytes"
	"errors"
	"fmt"
	"strconv"

	log "github.com/sirupsen/logrus"
)

type CPPLinter struct {
	baseLinter Linter
}

func CreateCPPLinter(parameters []string) ILinter {
	parameters = append(parameters, "./")
	return &CPPLinter{
		Linter{
			name:           "cpplint",
			parameters:     parameters,
			resultTemplate: []byte("Total errors found: "),
		},
	}
}

func (cppLint *CPPLinter) Run(directory string) ([]byte, error) {
	return cppLint.baseLinter.Run(directory)
}

func (cppLint *CPPLinter) ParseOutput(output []byte, err error) (int, error) {
	if len(output) == 0 {
		return 0, NoFilesError()
	}

	if err == nil {
		return 0, nil
	}

	ind := bytes.Index(output, cppLint.baseLinter.resultTemplate)
	if ind == -1 {
		log.WithFields(log.Fields{
			"resultTemplate": string(cppLint.baseLinter.resultTemplate),
		}).Error("Failed to find result template.")
		return -1, errors.New("output parsing failed")
	}
	fmt.Println(string(output[ind:]))
	strRes := string(output[ind+len(cppLint.baseLinter.resultTemplate) : len(output)-1])
	res, err := strconv.Atoi(strRes)
	if err != nil {
		log.WithFields(log.Fields{
			"strRes": strRes,
		}).Error("Failed to convert string to int.")
		return 0, err
	}
	return res, nil
}

func (cppLint *CPPLinter) CalculateResult(errors int) (int, error) {
	return errors, nil
}

func (cppLint *CPPLinter) CheckError(err error) error {
	return nil
}
