import { Box, Button, TextField, Typography, Avatar, Divider, Stack, Alert } from "@mui/material";
import { useState, useEffect } from "react";
import { useOutletContext } from "react-router-dom";
import SaveIcon from '@mui/icons-material/Save';
import InputAdornment from '@mui/material/InputAdornment';
import IconButton from '@mui/material/IconButton';
import Visibility from '@mui/icons-material/Visibility';
import VisibilityOff from '@mui/icons-material/VisibilityOff';

const SettingPage = () => {
    const [name, setName] = useState("");
    const [email, setEmail] = useState("");
    const [avatarUrl, setAvatarUrl] = useState("");
    const onUserUpdated = useOutletContext<any>();

    const [currentPassword, setCurrentPassword] = useState("");
    const [newPassword, setNewPassword] = useState("");
    const [confirmNewPassword, setConfirmNewPassword] = useState("");
    const [showPassword, setShowPassword] = useState(false);

    const [message, setMessage] = useState<{ type: 'success' | 'error', text: string } | null>(null);

    useEffect(() => {
        let userString = localStorage.getItem("user");
        if (!userString) {
            userString = sessionStorage.getItem("user");
        }
        if (userString) {
            const user = JSON.parse(userString);
            setName(user.name || "");
            setEmail(user.email || "");
            setAvatarUrl(user.avatarUrl || "");
        }
    }, []);

    const handleSave = (e: React.FormEvent) => {
        e.preventDefault();
        setMessage(null);

        const payload: any = { name };

        if (newPassword) {
            if (!currentPassword) {
                setMessage({ type: 'error', text: "Vui lòng nhập mật khẩu hiện tại để xác nhận thay đổi." });
                return;
            }
            if (newPassword !== confirmNewPassword) {
                setMessage({ type: 'error', text: "Mật khẩu mới không khớp." });
                return;
            }
            if (newPassword.length < 8) {
                setMessage({ type: 'error', text: "Mật khẩu mới phải có ít nhất 6 ký tự." });
                return;
            }

            payload.currentPassword = currentPassword;
            payload.newPassword = newPassword;
        }

        let storageType = 'session';
        let userString = sessionStorage.getItem("user");
        if (localStorage.getItem("user")) {
            storageType = 'local';
            userString = localStorage.getItem("user");
        }  
        
        if(userString) {
            const currentUser = JSON.parse(userString);
            const updatedUser = { ...currentUser, name: name };
            if (storageType === 'local') {
                localStorage.setItem("user", JSON.stringify(updatedUser));
            } else {
                sessionStorage.setItem("user", JSON.stringify(updatedUser));
            }
            if (onUserUpdated) {
                onUserUpdated(updatedUser);
            }
        }

        setMessage({ type: 'success', text: "Cập nhật thông tin thành công!" });

        setCurrentPassword("");
        setNewPassword("");
        setConfirmNewPassword("");
    };

return (
        <Box 
            sx={{ 
                width: '100%', 
                minHeight: '100%',
                bgcolor: 'background.paper',
                p: 4
            }}
        >
            <Typography variant="h5" fontWeight="bold" align="center" mb={4}>
                Cài đặt tài khoản
            </Typography>

            <form onSubmit={handleSave}>
                <Stack spacing={3}>
                    
                    <Box display="flex" flexDirection="column" alignItems="center">
                        <Avatar
                            src={avatarUrl}
                            alt={name}
                            sx={{ width: 100, height: 100, mb: 2, bgcolor: 'primary.main', fontSize: '2rem' }}
                        >
                            {name.charAt(0)?.toUpperCase()}
                        </Avatar>
                        <Button size="small">Đổi ảnh đại diện</Button>
                    </Box>

                    {message && (
                        <Alert severity={message.type}>{message.text}</Alert>
                    )}
                    
                    <Box display="grid" gridTemplateColumns={{ xs: '1fr', md: '1fr 1fr' }} gap={3}>
                        <TextField
                            label="Tên hiển thị"
                            value={name}
                            onChange={(e) => setName(e.target.value)}
                            fullWidth
                        />
                        <TextField
                            label="Email"
                            value={email}
                            disabled
                            helperText="Email không thể thay đổi"
                            fullWidth
                        />
                    </Box>

                    <Divider sx={{ my: 2 }}>
                        <Typography variant="caption" color="text.secondary">
                            BẢO MẬT
                        </Typography>
                    </Divider>

                    <TextField
                        label="Mật khẩu hiện tại"
                        type={showPassword ? "text" : "password"}
                        value={currentPassword}
                        onChange={(e) => setCurrentPassword(e.target.value)}
                        fullWidth
                        InputProps={{
                            endAdornment: (
                                <InputAdornment position="end">
                                    <IconButton
                                        onClick={() => setShowPassword(!showPassword)}
                                        edge="end"
                                    >
                                        {showPassword ? <Visibility /> : <VisibilityOff />}
                                    </IconButton>
                                </InputAdornment>
                            ),
                        }}
                    />
                    
                    <Box display="grid" gridTemplateColumns={{ xs: '1fr', md: '1fr 1fr' }} gap={3}>
                        <TextField
                            label="Mật khẩu mới"
                            type="text"
                            value={newPassword}
                            onChange={(e) => setNewPassword(e.target.value)}
                            fullWidth
                        />
                        
                        <TextField
                            label="Nhập lại mật khẩu mới"
                            type="password"
                            value={confirmNewPassword}
                            onChange={(e) => setConfirmNewPassword(e.target.value)}
                            fullWidth
                            required={!!newPassword}
                        />
                    </Box>

                    <Button 
                        variant="contained" 
                        size="large" 
                        type="submit" 
                        startIcon={<SaveIcon />}
                        sx={{ mt: 2, py: 1.5, width: { xs: '100%', md: 'auto' }, alignSelf: 'flex-end' }}
                    >
                        Lưu thay đổi
                    </Button>

                </Stack>
            </form>
        </Box>
    );
};

export default SettingPage;