        subroutine aa(A, SX, N)
          real A(N,N), SX(N)
            DO  I = 1, N-1
            DO K = I, N
              SX(K)=A(N,I)*A(N,K)
            END DO
            DO J = N-1, I, -1
               DO K = I, N
                  SX(K)=SX(K)+A(J,I)*A(J,K)
               END DO
            END DO

               ALPHA = SQRT (SX(I))
               IF (A(I,I).NE.0.) THEN
                  BETA = 1./ALPHA
                  DO J = I+1, N
                    A(J,I)=A(J,I)*BETA
                  END DO
                  SX(I) = A(I,I)*BETA+SIGN(1.,A(I,I))                     
                  A(I,I)=ALPHA
                  G=1./ABS(SX(I)) ! 1/gamma
                  DO K = I+1, N
                     SX(K)=SX(K)*BETA*G+SIGN(A(I,K),SX(I))
                     A(I,K) = A(I,K)+SX(K)*SX(I)
                     DO J = I+1, N
                       A (J,K) = A(J,K)-A(J,I)*SX(K)
                     END DO
                  END DO
               ELSE
                  IF (ALPHA.NE.0.) THEN
                     BETA = 1./ALPHA
                     DO J = I+1, N
                       A(J,I)=A(J,I)*BETA
                     END DO
                     SX(I) = -1. 
                     A(I,I)=ALPHA
                     G=-1.! 1/gamma
                     DO K = I+1, N
                        SX(K)=SX(K)*BETA*G+SIGN(A(I,K),SX(I))
                        A(I,K) = A(I,K)+SX(K)*SX(I)
                        DO J = I+1, N
                          A (J,K) = A(J,K)-A(J,I)*SX(K)
                        END DO
                     END DO
                  ELSE
                     SX(I)=1
                     G=2.
                     DO K = I+1, N
                        SX(K)=2.
                        A(I,K) = A(I,K)-SX(K)
                     END DO
                  END IF
               END IF               

		
	           END DO
          return
          end
