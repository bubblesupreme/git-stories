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

package linter

import (
	"bytes"
	"errors"
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
