export class ResponseFormat<T> {
    success: boolean;
    message: string;
    data?: T | null;
    timestamp: string;

    constructor(success: boolean, message: string, data?: T | null) {
        this.success = success;
        this.message = message;
        this.data = data || null;
        this.timestamp = new Date().toISOString();
    }

    static success<T>(data: T, message = 'Success'): ResponseFormat<T> {
        return new ResponseFormat(true, message, data);
    }

    static error<T>(data?: T, message = 'Error'): ResponseFormat<T> {
        return new ResponseFormat<T>(false, message, data);
    }
}