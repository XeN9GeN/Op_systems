#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define error_ret_val -1
#define FAILURE -1
#define SUCCESS 1
#define Daughter 0
#define Success_ret_val 0
#define EXIT_INVALID_CONFIG 5 

int global_var = 20;


int print_parent_initial_info(int local_var) {
	printf("[P] global var: adress = %p, value = %d\n", (void*)&global_var, global_var);
	printf("[P] local var: adress = %p, value = %d\n", (void*)&local_var, local_var);

	pid_t parent_pid = getpid();
	printf("[P] my pid = %d\n\n", parent_pid);

	if (fflush(stdout) != Success_ret_val) {
		perror("Error in fflush");
		return FAILURE;
	}
	return SUCCESS;
}

int run_child_logic(int local_var) {
	printf("[D] my pid = %d\n", getpid());
	printf("[D] parent pid = %d\n", getppid());

	printf("[D, before change] global var: adress = %p, value = %d\n", (void*)&global_var, global_var);
	printf("[D, before change] local var: adress = %p, value = %d\n", (void*)&local_var, local_var);

	global_var = 40;
	local_var = 20;

	printf("[D, after change] global var: adress = %p, value = %d\n", (void*)&global_var, global_var);
	printf("[D, after change] local var: adress = %p, value = %d\n", (void*)&local_var, local_var);

	if (fflush(stdout) != Success_ret_val) {
		perror("Error in fflush");
		return FAILURE;
	}

	exit(EXIT_INVALID_CONFIG);
}

int run_parent_logic(int local_var) {
	printf("[P] Sleep\n");
	sleep(15);
	printf("[P] Awake\n");

	printf("[P] global var: value = %d\n", global_var);
	printf("[P] local var:  value = %d\n", local_var);

	int status;
	pid_t child_pid = wait(&status);

	if (child_pid == error_ret_val) {
		if (errno == ECHILD) {
			printf("[P] Ошибка: у данного процесса больше нет дочерних процессов.\n");
		}
		else {
			perror("[P] Ошибка при вызове wait");
		}
	}

	else {
		if (WIFEXITED(status)) {
			printf("[P] Дочерний процесс %d завершился нормально.\n", child_pid);
			printf("[P] Код завершения = %d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status)) {
			printf("[P] Дочерний процесс %d был убит сигналом %d.\n", child_pid, WTERMSIG(status));
		}
	}
	return SUCCESS;
}

int main() {
	int local_var = 10;

	if (print_parent_initial_info(local_var) == FAILURE) {
		return FAILURE;
	}

	pid_t pid = fork();
	if (pid == error_ret_val) {
		perror("Ошибка fork");
		return FAILURE;
	}

	if (pid == Daughter) {
		if (run_child_logic(local_var) == FAILURE) {
			return FAILURE;
		}
	}
	else {
		if (run_parent_logic(local_var) == FAILURE) {
			return FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
