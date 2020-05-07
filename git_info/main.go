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

//go:generate protoc --go_out=$PWD/config --proto_path=$PWD/../protobuf/ config.proto
package main

import (
	"github.com/bubblesupreme/git-stories/git_info/worker"

	log "github.com/sirupsen/logrus"
	"github.com/spf13/cobra"
)

var mainCmd = &cobra.Command{
	Use:   "git_stories [parameters.json]",
	Short: "git_stories is git history visualization tool.",
	// Long: ``,
	Args: cobra.ExactValidArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		log.Info("git_stories called")
		params := args[0]
		worker, err := worker.CreateWorker()
		if err != nil {
			log.Fatal(err)
			return
		}

		err = worker.Work(params)
		if err != nil {
			log.Fatal(err)
			err = worker.ErrorCleanup()
			if err != nil {
				log.Fatal(err)
				return
			}
			return
		}
	},
}

func main() {
	if err := mainCmd.Execute(); err != nil {
		log.Fatal(err)
	}
}
