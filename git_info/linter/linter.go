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
