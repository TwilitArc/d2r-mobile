# D2R Mobile iOS 18 — r2

面向 iOS 18.4 及以上的实验版本。用户已确认 r2 在 **iPhone 15 Pro Max / iOS 18.4.1** 上正常运行。

## 改动

- 将 Metal 4 渲染提交与参数绑定改为旧版 Metal API，最低系统版本设为 iOS 18.4。
- 资源采用复制到 App 文件夹的导入方式，移除新版托管资源下载扩展。
- r2 补齐 `shader_map.tsv` 与 `pso_manifest.tsv`，修复上一版启动时“读不到着色器映射表”的中止；打包器会提前检查两张表是否存在。
- 构建号为 2，保留 Windows 本地打包支持。

## 下载与使用

- `D2RMobile-ios18-shell.ipa`：未签名的壳，不含游戏程序、游戏素材或游戏资源包，需自行打包后整包签名。
- `d2r-mobile-ios18-r2-source.zip`：对应版本的完整源码。GitHub 自动生成的 Source code 下载也可使用。
- `SHA256SUMS.txt`：上述两个文件的 SHA-256 校验值。

使用你自己的 Switch 版 3.3.94012 游戏文件，按仓库 `dm/tools/d2rm_pack.py` 的说明打包：

```text
python dm/tools/d2rm_pack.py D2RMobile-ios18-shell.ipa <游戏目录> D2RMobile.ipa
```

游戏目录包含 `ExeFS` 和 `RomFS`。打包电脑需要项目要求的 Python 依赖、.NET 和 spirv-cross 1.4.357.0 等工具；壳要求的着色器源码摘要为 `58f99c2ccd5d8661`，工具会检查是否匹配。没有 Apple 着色器编译器时，着色器源码随包携带，在手机首次使用时编译。

签名安装后，将你的游戏资源复制到「我的 iPhone / D2R Mobile」内的资源文件夹。覆盖更新时保持原包名和签名设置，以保留原资源与存档。

## 验证范围

macOS 完整编译、最低系统版本及 ZIP 完整性检查通过。实机正常运行结果来自用户反馈，目前确认的组合为 iPhone 15 Pro Max / iOS 18.4.1；其他设备与系统版本尚未验证。本自签壳不带 iCloud 同步、持续执行及提高内存上限权限。
