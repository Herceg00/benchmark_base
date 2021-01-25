        subroutine aa(A, SX, N)
          real A(N,N), SX(N) 
          	DO  I = 1, N-2

            DO K = I, N
              SX(K)=A(N,I)*A(N,K)
            END DO
            DO J = N-1, I+1, -1
                 SX(I)=SX(I)+A(J,I)*A(J,I)
            END DO 
            DO K = I+1, N
              DO J = N-1, K, -1
                 SX(K)=SX(K)+A(J,I)*A(J,K)
              END DO
              DO J = K-1,I+1,-1
                 SX(K)=SX(K)+A(J,I)*A(K,J)
              END DO
            END DO

               ALPHA = SQRT (SX(I))
               IF (A(I+1,I).NE.0.) THEN
                  BETA = 1./ALPHA
                  DO J = I+2, N
                    A(J,I)=A(J,I)*BETA
                  END DO
                  SX(I) = A(I+1,I)*BETA+SIGN(1.,A(I+1,I))                     
                  A(I+1,I)=ALPHA
                  G=1./ABS(SX(I)) ! 1/gamma
                  DO K = I+2, N
                     SX(K)=SX(K)*BETA*G+SIGN(A(K,I+1),SX(I))
                     A(K,K) = A(K,K)-2*A(K,I)*SX(K)
                  END DO
                  DO K = I+2, N                     
                     DO J = K+1, N
                       A (J,K) = A(J,K)-A(J,I)*SX(K)-A(K,I)*SX(I)
                     END DO
                  END DO
               ELSE
                  IF (ALPHA.NE.0.) THEN
                     BETA = 1./ALPHA
                     DO J = I+2, N
                       A(J,I)=A(J,I)*BETA
                     END DO
                     SX(I) = -1. 
                     A(I+1,I)=ALPHA
                     G=-1.! 1/gamma
                     DO K = I+2, N
                        SX(K)=SX(K)*BETA*G+SIGN(A(K,I+1),SX(I))
                        A(K,K) = A(K,K)-2*A(K,I)*SX(K)
                     END DO
                     DO K = I+2, N                     
                        DO J = K+1, N
                          A (J,K) = A(J,K)-A(J,I)*SX(K)-A(K,I)*SX(I)
                        END DO
                     END DO
                  ELSE
                     SX(I)=1
                  END IF
               END IF               

		
	         END DO
           return
          end
          