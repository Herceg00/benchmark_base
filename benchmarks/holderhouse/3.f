        subroutine aa(A, SX, SL, N)
          real A(N,N), SX(N), SL(N)

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
                     G=2
                     DO K = I+1, N
                        SX(K)=2
                        A(I,K) = A(I,K)-SX(K)
                     END DO
                  END IF
               END IF               

              IF (I.LT.N-1) THEN

            DO K = I, N
              SL(K)=A(I,N)*A(K,N)
            END DO
            DO J = N-1, I+1, -1
               DO K = I, N
                  SL(K)=SL(K)+A(I,J)*A(K,J)
               END DO
            END DO

               ALPHA = SQRT (SL(I))
               IF (A(I,I+1).NE.0.) THEN
                  BETA = 1./ALPHA
                  DO J = I+2, N
                    A(I,J)=A(I,J)*BETA
                  END DO
                  SL(I) = A(I,I+1)*BETA+SIGN(1.,A(I,I+1))                     
                  A(I,I+1)=ALPHA
                  G=1./ABS(SL(I)) ! 1/gamma
                  DO K = I+1, N
                     SL(K)=SL(K)*BETA*G+SIGN(A(K,I+1),SL(I))
                     A(K,I+1) = A(K,I+1)+SL(K)*SX(I)
                     DO J = I+2, N
                       A (K,J) = A(K,J)-A(I,J)*SL(K)
                     END DO
                  END DO
               ELSE
                  IF (ALPHA.NE.0.) THEN
                     BETA = 1./ALPHA
                     DO J = I+2, N
                       A(I,J)=A(I,J)*BETA
                     END DO
                     SL(I) = -1. 
                     A(I,I+1)=ALPHA
                     G=-1. ! 1/gamma
                     DO K = I+1, N
                        SL(K)=SL(K)*BETA*G+SIGN(A(K,I+1),SL(I))
                        A(K,I+1) = A(K,I+1)+SL(K)*SL(I)
                        DO J = I+2, N
                          A (K,J) = A(K,J)-A(I,J)*SL(K)
                        END DO
                     END DO
                  ELSE
                     SL(I)=1
                     G=2.
                     DO K = I+1, N
                        SL(K)=2.
                        A(K,I+1) = A(K,I+1)-SL(K)
                     END DO
                  END IF
               END IF   

              END IF
	           END DO
          return
          end
          