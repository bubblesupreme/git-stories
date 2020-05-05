package main

import (
	"fmt"

	"./worker"
)

func main() {

	worker, err := worker.CreateWorker()
	if err != nil {
		fmt.Println(err)
		return
	}

	err = worker.Work("./parameters.json")
	if err != nil {
		fmt.Println(err)
		return
	}
}
