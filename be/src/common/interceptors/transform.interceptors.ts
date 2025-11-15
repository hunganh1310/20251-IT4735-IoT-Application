import { CallHandler, ExecutionContext, Injectable, NestInterceptor } from "@nestjs/common";
import { ResponseFormat } from "../response-format";
import { map, Observable } from "rxjs";

@Injectable()
export class TransformInterceptor<T> implements NestInterceptor<T, ResponseFormat<T>> {
    intercept(context: ExecutionContext, next: CallHandler<T>): Observable<ResponseFormat<T>> | Promise<Observable<ResponseFormat<T>>> {
        return next.handle().pipe(
            map((data) => {
                return ResponseFormat.success(data);
            }),
        );
    }
}