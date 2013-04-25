// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include	<stdio.h>
#include	<pthread.h>
#include	<semaphore.h>
#ifdef __AVM2__
#include <AS3/AVM2.h>
#define printf tprintf
#endif

#define	NPHIL	8
static sem_t	forks[NPHIL];
typedef struct {
	int	id, left_fork, right_fork;
} philo_t;
static philo_t	philo_args[NPHIL];
static void	*philo(void*);
extern void	*sometask(void*);
int main()
{
	int	i;
	pthread_t thread;
	for(i = 0; i < NPHIL; i++){
		(void)sem_init(&forks[i], 0, 1);
		philo_args[i].id	= i;
		philo_args[i].left_fork	= i;
		philo_args[i].right_fork	= (i+1)%NPHIL;
	}
	for(i = 0; i < NPHIL; i++)
		pthread_create(&thread, NULL, philo, &philo_args[i]);
	for(;;) sometask(NULL);
	return 0;
}
static void *philo(void *philo_arg)
{
	philo_t	*argp	= (philo_t *)philo_arg;
	int	id	= argp->id;
	int	left	= argp->left_fork;
	int	rght	= argp->right_fork;
	(void)printf("id %d left %d rght %d\n",
			id, left, rght);
	for(;;){
		(void)sem_wait(&forks[left]);
		(void)sem_wait(&forks[rght]);
		(void)printf("philo %d eating w. %d and %d\n", id, left, rght);
		(void)sometask(NULL); /* eating */
		(void)printf("philo %d done   w. %d and %d\n", id, left, rght);
		(void)sem_post(&forks[left]);
		(void)sem_post(&forks[rght]);
		(void)sometask(NULL); /* think */
	} /* NOTREACHED */
}

void *sometask(void *arg)
{
#ifdef __AVM2__
	unsigned dummy = 1;
	avm2_msleep(&dummy, &dummy, 300);
#else
  sleep(1);
#endif
}
